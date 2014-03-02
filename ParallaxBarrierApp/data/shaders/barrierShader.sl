uniform float A;
uniform float B;
uniform float Xo;
uniform float leftEyePositionX;
uniform float leftEyePositionY;
uniform float pixelWidth;
uniform float pixelCoveragePercentageEpsilon;
uniform float inverted;
 
void main(){

	float pixelStartX = (gl_FragCoord.x - 0.5) * pixelWidth;
	float pixelEndX = (gl_FragCoord.x + 0.5) * pixelWidth;

	float leftShutterDistanceFraction = 1/leftEyePositionY;

	float startNextIterationX;
	float endPrevIterationX;

	float pixelStartN;
	float pixelEndN;

	if (A == 1.0)
	{
		pixelStartN = ( ((pixelStartX - leftEyePositionX * leftShutterDistanceFraction) / (1 - leftShutterDistanceFraction)) - Xo) / B;
		pixelEndN = ( ((pixelEndX - leftEyePositionX * leftShutterDistanceFraction) / (1 - leftShutterDistanceFraction)) - Xo) / B;
		startNextIterationX = leftEyePositionX * leftShutterDistanceFraction + (Xo + B * (floor(pixelStartN) + 1.0)) * (1 - leftShutterDistanceFraction);
		endPrevIterationX = leftEyePositionX * leftShutterDistanceFraction + (Xo + B * floor(pixelEndN)) * (1 - leftShutterDistanceFraction);
	} 
	else 
	{
		pixelStartN = log( (( ((pixelStartX - leftEyePositionX * leftShutterDistanceFraction) / (1 - leftShutterDistanceFraction)) * (1 - A)) - B )/ (Xo - B - (A*Xo)) ) / log(A);
		pixelEndN = log( (( ((pixelEndX - leftEyePositionX * leftShutterDistanceFraction) / (1 - leftShutterDistanceFraction)) * (1 - A)) - B )/ (Xo - B - (A*Xo)) ) / log(A);
		float startNextIterationAn = pow(A, floor(pixelStartN) + 1.0);
		float endPrevIterationAn = pow(A, floor(pixelEndN));
		startNextIterationX = leftEyePositionX * leftShutterDistanceFraction + (startNextIterationAn * Xo + (B * (1 - startNextIterationAn) ) / (1 - A)) * (1 - leftShutterDistanceFraction);
		endPrevIterationX = leftEyePositionX * leftShutterDistanceFraction + (endPrevIterationAn * Xo + (B * (1 - endPrevIterationAn) ) / (1 - A)) * (1 - leftShutterDistanceFraction);
	}

	float pixelStartCoveredPercentage = (startNextIterationX - pixelStartX) / (pixelEndX - pixelStartX);
	float pixelEndCoveredPercentage = (pixelEndX - endPrevIterationX) / (pixelEndX - pixelStartX);

	if ( (floor(pixelStartN) == floor(pixelEndN) && mod(floor(pixelStartN), 2.0) == 0.0)
		|| (floor(pixelStartN) < floor(pixelEndN) && mod(floor(pixelStartN), 2.0) == 0.0 &&  pixelStartCoveredPercentage > 1.0 - pixelCoveragePercentageEpsilon )
		|| (floor(pixelStartN) < floor(pixelEndN) && mod(floor(pixelEndN), 2.0) == 0.0 &&  pixelEndCoveredPercentage > 1.0 - pixelCoveragePercentageEpsilon ) 
		)
	{
		gl_FragColor = inverted == 0.0? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0, 0, 0, 1.0);
	}
	else if ( (floor(pixelStartN) == floor(pixelEndN) && !(mod(floor(pixelStartN), 2.0) == 0.0))
		|| (floor(pixelStartN) < floor(pixelEndN) && !(mod(floor(pixelStartN), 2.0) == 0.0) &&  pixelStartCoveredPercentage > 1.0 - pixelCoveragePercentageEpsilon )
		|| (floor(pixelStartN) < floor(pixelEndN) && !(mod(floor(pixelEndN), 2.0) == 0.0) &&  pixelEndCoveredPercentage > 1.0 - pixelCoveragePercentageEpsilon ) 
		)
	{
		gl_FragColor = inverted == 0.0? vec4(0, 0, 0, 1.0) : vec4(1.0, 1.0, 1.0, 1.0); 
	}
	else
	{
		gl_FragColor = vec4(0, 0, 0, 1.0);
	}
}