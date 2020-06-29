
uniform sampler2D baseTexture;
uniform float glowFactor;
void main(void)
{
   vec2 texcoord = vec2(gl_TexCoord[0]);
    const vec4 highlightColor = vec4(1.0, 0.5, 0.0, 1.0);
    vec4 texColor = texture2D( baseTexture, texcoord);
    gl_FragColor = mix (texColor , highlightColor  , glowFactor) ;
}
