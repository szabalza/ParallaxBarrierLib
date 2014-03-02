uniform sampler2DRect leftTexture;
uniform sampler2DRect rightTexture;

uniform float A;
uniform float B;
uniform float Xo;
uniform float pixelWidth;
uniform float pixelCoveragePercentageEpsilon;
uniform float inverted;
 
void main(){

	float pixelStartX = (gl_FragCoord.x - 0.5) * pixelWidth;
	float pixelEndX = (gl_FragCoord.x + 0.5) * pixelWidth;
	float startNextIterationX;
	float endPrevIterationX;

	float pixelStartN;
	float pixelEndN;

	if (A == 1.0)
	{
		pixelStartN = (pixelStartX - Xo) / B;
		pixelEndN = (pixelEndX - Xo) / B;
		startNextIterationX = Xo + B * (floor(pixelStartN) + 1.0);
		endPrevIterationX = Xo + B * floor(pixelEndN);
	} 
	else 
	{
		pixelStartN = log( ((pixelStartX * (1 - A)) - B )/ (Xo - B - (A*Xo)) ) / log(A);
		pixelEndN = log( ((pixelEndX * (1 - A)) - B )/ (Xo - B - (A*Xo)) ) / log(A);
		float startNextIterationAn = pow(A, floor(pixelStartN) + 1.0);
		float endPrevIterationAn = pow(A, floor(pixelEndN));
		startNextIterationX = startNextIterationAn * Xo + (B * (1 - startNextIterationAn) ) / (1 - A);
		endPrevIterationX = endPrevIterationAn * Xo + (B * (1 - endPrevIterationAn) ) / (1 - A);
	}

	float pixelStartCoveredPercentage = (startNextIterationX - pixelStartX) / (pixelEndX - pixelStartX);
	float pixelEndCoveredPercentage = (pixelEndX - endPrevIterationX) / (pixelEndX - pixelStartX);

	if ( (floor(pixelStartN) == floor(pixelEndN) && mod(floor(pixelStartN), 2.0) == 0.0)
		|| (floor(pixelStartN) < floor(pixelEndN) && mod(floor(pixelStartN), 2.0) == 0.0 &&  pixelStartCoveredPercentage > 1.0 - pixelCoveragePercentageEpsilon )
		|| (floor(pixelStartN) < floor(pixelEndN) && mod(floor(pixelEndN), 2.0) == 0.0 &&  pixelEndCoveredPercentage > 1.0 - pixelCoveragePercentageEpsilon ) 
		)
	{
		gl_FragColor = inverted == 0.0? texture(leftTexture, gl_FragCoord.xy) : texture(rightTexture, gl_FragCoord.xy);
	}
	else if ( (floor(pixelStartN) == floor(pixelEndN) && !(mod(floor(pixelStartN), 2.0) == 0.0))
		|| (floor(pixelStartN) < floor(pixelEndN) && !(mod(floor(pixelStartN), 2.0) == 0.0) &&  pixelStartCoveredPercentage > 1.0 - pixelCoveragePercentageEpsilon )
		|| (floor(pixelStartN) < floor(pixelEndN) && !(mod(floor(pixelEndN), 2.0) == 0.0) &&  pixelEndCoveredPercentage > 1.0 - pixelCoveragePercentageEpsilon ) 
		)
	{
		gl_FragColor = inverted == 0.0? texture(rightTexture, gl_FragCoord.xy) : texture(leftTexture, gl_FragCoord.xy); 
	}
	else
	{
		gl_FragColor = vec4(0, 0, 0, 1.0);
	}
}