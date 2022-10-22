#version 430 core

layout (quads, equal_spacing) in; 

uniform int N,M; //stupanj!

const int nmax=20;

vec4 bezier(int k, vec4 P[nmax], float t)
{
	vec4 rez;
	int i,j;
    
    for(j=1; j<=k; ++j)
		for(i=k; i>=j; --i)
			P[i]=mix(P[i-1],P[i],t); 
	rez=P[k];  
	
	return rez;
}

vec4 evaluate_patch(vec2 at)
{
    vec4 Q[nmax],P[nmax];
    int i,j;

    for (i = 0; i <= M; i++)
    {
		for (j = 0; j<=N; j++) 
			Q[j]=gl_in[i + j*(M+1)].gl_Position;
        P[i] = bezier(N,Q,at.y);
    }

    return bezier(M,P,at.x);
}

const float epsilon = 0.001;

void main(void)
{
    vec4 p1 = evaluate_patch(gl_TessCoord.xy);

	gl_Position = p1;

}
