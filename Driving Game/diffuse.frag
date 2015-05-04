#version 400

in vec4 basecolour;
in vec2 texture_coordinates; //texture coordinates from vertex shader
in vec3 Normal0;


struct DirectionalLight
{
    vec3 Colour;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3 Direction;
};
                                               
uniform DirectionalLight gDirectionalLight;
uniform sampler2D basic_texture;              

out vec4 frag_colour;                                    
                                                                                    
void main()                                                                         
{                                                            
    vec4 AmbientColour = vec4(0.2, 0.2, 0.2, 0.2); //vec4(gDirectionalLight.Color, 1.0f) *                       
                        //gDirectionalLight.AmbientIntensity;                        
                                                                                    
    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);    
                                                                                    
    vec4 DiffuseColour;                                                              
                                                                                    
    if (DiffuseFactor > 0) {                                                        
        DiffuseColour = vec4(gDirectionalLight.Colour, 1.0f) *                        
                       gDirectionalLight.DiffuseIntensity *                         
                       DiffuseFactor;                                               
    }                                                                               
    else {                                                                          
        DiffuseColour = vec4(0, 0, 0, 0);                                            
    }                                                          
	                                                                 
    frag_colour = texture2D(basic_texture, texture_coordinates) * (AmbientColour + DiffuseColour); 
	frag_colour = vec4(frag_colour.xyz, 1) * basecolour; //Force the alpha channel to 1 because we don't want lighting to affect the opacity of objects
}   