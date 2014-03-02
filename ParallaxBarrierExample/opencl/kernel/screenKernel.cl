__kernel void updateScreenPixels(	const __global char* screenPoints, 
									__read_only image2d_t leftImage, __read_only image2d_t rightImage, 
									__write_only image2d_t screenImage)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);

	const int screenImageWidth = get_image_width(screenImage);
	const int screenImageHeight = get_image_height(screenImage);

	if (i < screenImageWidth && j < screenImageHeight)
	{
		int2 coord = (int2) (i, j);
		
		float4 color;
		if (screenPoints[i] == -1)
		{
			color = read_imagef(leftImage, coord);
		} 
		else if (screenPoints[i] == 1)
		{
			color = read_imagef(rightImage, coord);
		}
		else 
		{
			color = (float4) (0,0,0,1);
		}

		write_imagef(screenImage, coord, color);
	}

}