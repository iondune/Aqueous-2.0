
#pragma once

#include <ionEngine.h>


class ITerrainHeightSource
{

public:

	virtual bool IsPointInBounds(vec2f const & Position) = 0;
	virtual float GetHeightAtPoint(vec2f const & Position) = 0;

};

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

	CBathymetryRasterizer();

	void ConvertAndRasterize(bool const Processing);

	void ClassifyGroups();
	void DetectBridgeGroups();
	void FillBridgeGroups();
	void FillGroups();
	void RasterizeImage();

	bool IsPointInBounds(vec2f const & Position);
	float GetHeightAtPoint(vec2f const & Position);

	void WriteToFile(string const & FileName);
	void ReadFromFile(string const & FileName);

protected:

	struct SPixelBucket
	{
		int Count = 0;
		float Sum = 0;
		int Tag = 0;
		bool Approximate = false;

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
		bool IsBridge = false;
	};
	STagInfo * TagGroups = nullptr;


	void CopySourcePointsToBuckets();

	int Helper_CheckBucketTag(vector<vec2i> & Queue, int const u, int const v, int const tag);
	int Helper_FillBucketTags(int const i, int const j, int const tag);

	vector<vec2i> Helper_GetAllMatchingGroup(int const Tag);
	void Helper_EstimatePixelValue(vec2i const & index, int const KernelSize);
	void Helper_EstimatePixelValueBridge(vec2i const & index, int const KernelSize, int const MinSamples);
	void Helper_ReconstructTagGroup(STagInfo & Group, int const Tag);

};

class CTopographyRasterizer
{

public:

	struct SPixelBucket
	{
		float Value = 0;
		bool Interior = false;
	};
	SPixelBucket * Buckets = nullptr;

	vector<vec2f> SourceLongLatPostings;
	int ImageSize = 4096;
	float RegionXCorner = 33.435f;
	float RegionYCorner = -118.5f;
	float RegionXSize = 0.03f;
	float RegionYSize = 0.03f;
	string OutputName = "Output.png";

	void FillInteriorPoints();
	void RasterizeImage();

	void WriteToFile(string const & FileName);
	void ReadFromFile(string const & FileName);

protected:

	SPixelBucket * Helper_GetBucket(int const i, int const j);
	SPixelBucket * Helper_GetBucket(vec2i const & index);

	float Helper_ClosestEdgeDistance(vec2f const & Point);

};
