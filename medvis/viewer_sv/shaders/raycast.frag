#version 330

smooth in vec3 tex_coords;
smooth in vec3 position;

smooth in vec3 camera;

uniform sampler3D volume;

uniform float minT_R;
uniform float maxT_R;

uniform float minT_G;
uniform float maxT_G;

uniform float minT_B;
uniform float maxT_B;

uniform vec3 lightPos;
out vec4 frag_color;

int mode = 0;

float delta = 1.0/512;

vec4 transfer(float dst) {
  vec4 result = vec4(0,0,0,0);

  // Apply RGB based on user intervals
  if (dst >= minT_R && dst <= maxT_R) 
    result += vec4(dst, 0, 0, dst/3);
  if (dst >= minT_G && dst <= maxT_G) 
    result += vec4(0, dst, 0, dst/3);
  if (dst >= minT_B && dst <= maxT_B) 
    result += vec4(0, 0, dst, dst/3);
  return result;
}

vec3 calc_normal(vec3 curr_pos) {
  // Fetch gradient values for normal calculation
  vec4 xcomp1  = texture(volume, curr_pos - vec3(delta,0,0));
  vec4 xcomp2  = texture(volume, curr_pos + vec3(delta,0,0));

  vec4 ycomp1  = texture(volume, curr_pos - vec3(0,delta,0));
  vec4 ycomp2  = texture(volume, curr_pos + vec3(0,delta,0));

  vec4 zcomp1  = texture(volume, curr_pos - vec3(0,0,delta));
  vec4 zcomp2  = texture(volume, curr_pos + vec3(0,0,delta));
  return normalize(vec3((xcomp1-xcomp2).x, (ycomp1-ycomp2).x, (zcomp1-zcomp2).x));
}

float calc_opacity(vec3 curr_pos) {
  
  // Define initial parameters
  int max_iter = 50;
  float out_opa = 1.0f;
  vec3 dir = lightPos - (curr_pos - vec3(0.5));
  dir = normalize(dir);
  // Step size is smaller than outer loop
  vec3 step_size = vec3(delta * 100);
  
  vec4 color;
  vec4 transfered_color;
  vec3 iter_pos = curr_pos;
  float density;

  // Inner loop
  for (int i = 0; i < max_iter; i++) {
    // Fetch color and transfer it
    color = texture(volume, iter_pos);
    transfered_color = transfer(color.x);
    density = transfered_color.a;

    // Apply back to front compositing
    out_opa -= density * (out_opa);

    iter_pos = iter_pos + dir * step_size;

    // Check if out of bounds or if we reached a
    // completely transparent region.
    vec3 temp1 = sign(iter_pos - vec3(0));
    vec3 temp2 = sign(vec3(1) - iter_pos);
    float inside = dot(temp1, temp2);

    if (inside < 3.0 || out_opa <= 0.0f)
      break;

  }
  return out_opa;
}

void main (void) {
  
  // Initialise output color
  vec4 dst = vec4(0,0,0,0);
  
  // Initialise raycasting parameters
  vec3 pos = tex_coords.xyz;
  vec3 stepsize = vec3(delta);
  vec3 direction = position - camera;
  direction = normalize(direction);

  // Define temporary values
  vec4 color;
  float density;
  vec4 transfered_color;
  float opacity;

  // Define color of the light
  vec3 lightColor = vec3(1,1,1);

  // Internal loop
  int i;
  for (i=0; i < 512; i++) {
    // Fetch color and apply transfer function to it
    color = texture(volume, pos);
    transfered_color = transfer(color.x);
    density = transfered_color.a;

    // Calculate normal 
    vec3 N = calc_normal(pos);

    //Calculate light direction
    vec3 L = normalize(lightPos - pos-vec3(0.5));

    // Calculate diffuse component
    float diff = max(dot(N,L),0.0);
    vec3 diffuse = diff * lightColor;

    // Calculate specular component
    vec3 reflectDir = reflect(-L, N);  
    float spec = pow(max(dot(direction, reflectDir), 0.0), 64);
    vec3 specular = spec * lightColor;

    // Apply Phong Shading
    transfered_color.rgb = (diffuse + specular) * transfered_color.rgb;

    // Apply advanced lighting
    opacity = calc_opacity(pos);

    // Add color and transparency to output
    dst.rgb += transfered_color.rgb * density * (1.0-dst.a) * (1.0-opacity);
    dst.a += density * (1.0-dst.a);

    // Keep raycasting
    pos = pos + direction * stepsize;

    // Check if out of bounds
    vec3 temp1 = sign(pos - vec3(0));
    vec3 temp2 = sign(vec3(1) - pos);
    float inside = dot(temp1, temp2);

    if (inside < 3.0)
      break;
  }
  frag_color = dst;
}
