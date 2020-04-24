
// specular light calculate
float Phong(vec3 viewDir,vec3 reflectDir,float shininess){
    return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
}
float Blinn_Phong(vec3 viewDir,vec3 lightDir,vec3 Normal,float shininess){
    vec3 halfVector = normalize(viewDir+lightDir);
    return pow(max(dot(Normal, halfVector), 0.0), shininess);
}

// from screen depth to linear 01
float depthToEyeCoord(float ScreenDepth,float near,float far){
    float NDCDepth = ScreenDepth * 2 - 1;
    float depth = (2.0 * near * far) / (far + near - NDCDepth * (far - near));   
    return depth/far;
}