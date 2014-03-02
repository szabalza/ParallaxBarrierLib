__kernel void updateBarrierPixels(	const __global char* barrierPoints,
									__write_only image2d_t barrierImage)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);

	const int barrierImageWidth = get_image_width(barrierImage);
	const int barrierImageHeight = get_image_height(barrierImage);

	if (i < barrierImageWidth && j < barrierImageHeight)
	{
		int2 coord = (int2) (i, j);
		
		float4 color;
		if (barrierPoints[i] == 1)
		{
			color = (float4) (1.f, 1.f, 1.f, 1.f);
		} 
		else 
		{
			color = (float4) (0.f, 0.f, 0.f, 1.f);
		}

		write_imagef(barrierImage, coord, color);
	}

}