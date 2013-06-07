// Radome fragment shader

uniform samplerCube EnvMap;
varying vec3 ReflectDir;
varying vec4 position;

uniform sampler2DRect video;
uniform vec2 videoSize;
uniform float videoMix;
uniform int mixMode;
uniform int mappingMode;
uniform float domeHeight;
uniform float domeDiameter;

vec2 getUV() {
    vec2 normalUV;
    if (mappingMode == 0) {
        // Basic latitude/longitude mapping
        normalUV = vec2(0.5 + atan(position.x, position.z)/(2.0*3.141592),
                        4.0 * asin(position.y/domeHeight)/(2.0*3.141592));
    } else if (mappingMode == 1) {
        // Mirrored quadrants
        normalUV = vec2(abs(position.x)/domeDiameter,
                        4.0 * asin(position.y/domeHeight)/(2.0*3.141592));
    } else if (mappingMode == 2) {
        // Fisheye
        float radians = atan(position.x, position.z);
        float radius = 1.0 - asin(position.y/domeHeight)/(2.0*3.141592);
        float dist = videoSize.y * 0.5 * radius * 0.2;
        return vec2(0.5 * videoSize.x + dist * sin(radians), 0.5 * videoSize.y + dist * cos(radians));
    } else {
        // Default UV
        normalUV = gl_TexCoord[0].st;
    }
    
    return vec2(normalUV.s * videoSize.x, normalUV.t * videoSize.y);
}

vec4 mixColors(vec4 envColor, vec4 videoColor, float videoMix) {
    if (mixMode == 0) {
        // Underlay
        return mix(videoColor, envColor, envColor.a * videoMix);
    } else if (mixMode == 1) {
        // Overlay
        return mix(envColor, videoColor, videoColor.a * (1.0-videoMix));
    } else if (mixMode == 2) {
        // Mask
        return mix(envColor, videoColor, envColor.a * (1.0-videoMix));
    } else {
        return vec4(0.0,0.0,0.0,0.0);
    }
}

/*
vec4 multiLayerMix(vec4 envColor,
                   vec4 videoOverlayColor, float videoOverlayMix,
                   vec4 videoUnderlayColor, float videoUnderlayMix,
                   vec4 videoMaskColor, float videoMaskMix)
{
    vec4 mask = mix(envColor, videoMaskColor, envColor.a * videoMaskMix);
    vec4 under = mix(videoUnderlayColor, mask, envColor.a * videoUnderlayMix);
    vec4 over = mix(under, videoOverlayColor, videoOverlayColor.a * videoOverlayMix);
    return over;
}
*/

void main()
{
    // clip below the y-plane
    if (position.y < -1.5) discard;
    
    // draw the y-plane green
    if (position.y < 0.1) {
        float distSquared = pow(position.x, 2.0) + pow(position.z, 2.0);
        if (distSquared < pow(domeDiameter/2.0, 2.0) ) { // unless it's under the dome
            discard;
        } else {
            gl_FragColor = vec4(0.15, 0.75, 0.3, 1.0);
        }
    } else {
        
        // everywhere else, reflect the environment map back onto the dome
        vec3 lookupVec = ReflectDir;
 
        // Look up texture pixel in cube map
        vec4 color = textureCube(EnvMap, lookupVec);

        if (videoMix >= 0.0) {
            // Get 2D video overlay color from the mapping mode and the input video texture
            vec2 uv = getUV();
            vec4 videoColor = texture2DRect(video, uv);

            // Blend according to the mix mode and mix level
            color = mixColors(color, videoColor, videoMix);
        }
        
        // Blend any remaining alpha against black
        gl_FragColor = mix(vec4(0.0,0.0,0.0,1.0), color, color.a);
        gl_FragColor.a = 1.0;
    }
}

