
//This function assumes a light source at infinite distance
//Returns the resulting lit color
//Ia - Scene ambient light
//Ld,Ls - Light's diffuse, specular emission
//kd,ks - Objects diffuse, specular reflection
//shininess - object's shininess
//n - normal vector
//l - light vector (points towards light)
//v - view vector (points towards camera)

float3 CalcPhongShading(float3 Ia, float3 Ld, float3 Ls, float3 kd, float3 ks, float shininess, float3 n, float3 l, float3 v)
{
    //Half vector between l and v
    float3 h = normalize(l + v);
    float3 Id = kd * max(0, dot(n, l)) * Ld;
    float3 Is = ks * pow(abs(dot(n, h)), shininess) * Ls;

    return Ia + Id + Is;
}
