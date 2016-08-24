
#pragma once

#include <ionEngine.h>


class CBathymetryRasterizer
{

public:

	vector<vec3f> SourceElevationPostings;
	int ImageSize = 4096;
	float RegionXCorner = 33.435f;
	float RegionYCorner = -118.5f;
	float RegionXSize = 0.03f;
	float RegionYSize = 0.03f;
	string OutputName = "Output.png";

	void ConvertAndRasterize();

protected:

	struct SPixelBucket
	{
		int Count = 0;
		float Sum = 0;
		int Tag = 0;

		float GetValue() const
		{
			return Sum / (float) Count;
		}
	};
	SPixelBucket * Buckets = nullptr;

	SPixelBucket * Helper_GetBucket(int const i, int const j);
	SPixelBucket * Helper_GetBucket(vec2i const & index);

	struct STagInfo
	{
		int count = 0;
		int start_x = -1;
		int start_y = -1;
	};
	STagInfo * TagGroups = nullptr;


	void CopySourcePointsToBuckets();

	int Helper_CheckBucketTag(vector<vec2i> & Queue, int const u, int const v, int const tag);
	int Helper_FillBucketTags(int const i, int const j, int const tag);
	void ClassifyGroups();

	vector<vec2i> Helper_GetAllMatchingGroup(int const Tag);
	void Helper_EstimatePixelValue(vec2i const & index);
	void Helper_ReconstructTagGroup(STagInfo & Group, int const Tag);
	void FillGroups();

	void RasterizeImage();

};
