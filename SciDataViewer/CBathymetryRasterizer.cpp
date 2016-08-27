
#include "CBathymetryRasterizer.h"
#include "CatalinaOutline.h"

#include <ionScience.h>

using namespace ion;


void CBathymetryRasterizer::ConvertAndRasterize()
{
	static uint const ImageSize = 4096;

	Buckets = new SPixelBucket[ImageSize * ImageSize];

	CopySourcePointsToBuckets();
	//ClassifyGroups();
	//FillGroups();
	RasterizeImage();
}

bool CBathymetryRasterizer::IsPointInBounds(vec2f const & Position)
{
	if ((Position.X >= RegionXCorner && Position.X <= RegionXCorner + RegionXSize) &&
		(Position.Y >= RegionYCorner && Position.Y <= RegionYCorner + RegionYSize))
	{
		vec2f RealIndex = vec2f(
			(Position.X - RegionXCorner) / RegionXSize,
			(Position.Y - RegionYCorner) / RegionYSize);

		int i = Clamp((int) (RealIndex.X * ImageSize), 0, ImageSize - 1);
		int j = Clamp((int) (RealIndex.Y * ImageSize), 0, ImageSize - 1);

		auto Bucket = Helper_GetBucket(i, j);

		if (Bucket && Bucket->Count)
		{
			return true;
		}
	}

	return false;
}

float CBathymetryRasterizer::GetHeightAtPoint(vec2f const & Position)
{
	vec2f const RealIndex = vec2f(
		(Position.X - RegionXCorner) / RegionXSize,
		(Position.Y - RegionYCorner) / RegionYSize) * (float) ImageSize;

	int i = Clamp((int) RealIndex.X, 0, ImageSize - 1);
	int j = Clamp((int) RealIndex.Y, 0, ImageSize - 1);

	auto Bucket = Helper_GetBucket(i, j);
	if (Bucket && Bucket->Count)
	{
		float Value = Bucket->GetValue();

		float Data[2][2] =
		{
			{ Value, Value },
			{ Value, Value },
		};

		for (int m = 0; m < 2; ++ m)
		{
			for (int n = 0; n < 2; ++ n)
			{
				auto Bucket = Helper_GetBucket(i + m, j + n);
				if (Bucket && Bucket->Count)
				{
					Data[m][n] = Bucket->GetValue();
				}
			}
		}

		float integral;
		return BilinearInterpolate(Data, modf(RealIndex.X, &integral), modf(RealIndex.Y, &integral));
	}
	else
	{
		return std::numeric_limits<float>::quiet_NaN();
	}

}

CBathymetryRasterizer::SPixelBucket * CBathymetryRasterizer::Helper_GetBucket(int const i, int const j)
{
	if (i >= 0 && i < ImageSize &&
		j >= 0 && j < ImageSize)
	{
		return & Buckets[ImageSize * i + j];
	}
	else
	{
		return nullptr;
	}
}

CBathymetryRasterizer::SPixelBucket * CBathymetryRasterizer::Helper_GetBucket(vec2i const & index)
{
	return Helper_GetBucket(index.X, index.Y);
}

void CBathymetryRasterizer::CopySourcePointsToBuckets()
{
	CStopWatch sw;
	sw.Start();

	for (auto const & Point : SourceElevationPostings)
	{
		int const IndexX = (int) (((Point.X - RegionXCorner) / RegionXSize) * ImageSize);
		int const IndexY = (int) (((Point.Y - RegionYCorner) / RegionYSize) * ImageSize);

		if (IndexX >= 0 && IndexX < ImageSize &&
			IndexY >= 0 && IndexY < ImageSize)
		{
			Buckets[ImageSize * IndexX + IndexY].Sum += Point.Z;
			Buckets[ImageSize * IndexX + IndexY].Count ++;
		}
	}

	Log::Info("Copy to buckets took %.3f", sw.Stop());
}

int CBathymetryRasterizer::Helper_CheckBucketTag(vector<vec2i>& Queue, int const u, int const v, int const tag)
{
	if (u >= 0 && v >= 0 && u < ImageSize && v < ImageSize)
	{
		SPixelBucket & Bucket = Buckets[ImageSize * u + v];
		if (Bucket.Count == 0 && Bucket.Tag == 0)
		{
			Bucket.Tag = tag;
			Queue.push_back(vec2i(u, v));

			return 1;
		}
	}

	return 0;
}

int CBathymetryRasterizer::Helper_FillBucketTags(int const i, int const j, int const tag)
{
	int NumberFilled = 0;
	vector<vec2i> Queue;

	Queue.push_back(vec2i(i, j));

	while (! Queue.empty())
	{
		vec2i const Current = Queue.back();
		Queue.pop_back();

		int const u = Current.X;
		int const v = Current.Y;

		NumberFilled += Helper_CheckBucketTag(Queue, u + 1, v, tag);
		NumberFilled += Helper_CheckBucketTag(Queue, u - 1, v, tag);
		NumberFilled += Helper_CheckBucketTag(Queue, u, v + 1, tag);
		NumberFilled += Helper_CheckBucketTag(Queue, u, v - 1, tag);
	}

	return NumberFilled;
}

void CBathymetryRasterizer::ClassifyGroups()
{
	CStopWatch sw;
	sw.Start();

	int tag = 1;
	TagGroups = new STagInfo[ImageSize * ImageSize]();
	for (int i = 0; i < ImageSize; ++ i)
	{
		for (int j = 0; j < ImageSize; ++ j)
		{
			TagGroups[tag].count = Helper_FillBucketTags(i, j, tag);
			if (TagGroups[tag].count)
			{
				TagGroups[tag].start_x = i;
				TagGroups[tag].start_y = j;
				tag ++;
			}
		}
	}
	tag = 1;
	while (TagGroups[tag].count)
	{
		Log::Info("    Group %d has %d members", tag, TagGroups[tag].count);
		tag ++;
	}

	Log::Info("Classify groups took %.3f", sw.Stop());
}

vector<vec2i> CBathymetryRasterizer::Helper_GetAllMatchingGroup(int const Tag)
{
	vector<vec2i> Group;

	for (int i = 0; i < ImageSize; ++ i)
	{
		for (int j = 0; j < ImageSize; ++ j)
		{
			SPixelBucket * Bucket = Helper_GetBucket(i, j);
			if (Bucket->Tag == Tag)
			{
				Group.push_back(vec2i(i, j));
			}
		}
	}

	return Group;
}

void CBathymetryRasterizer::Helper_EstimatePixelValue(vec2i const & index)
{
	static int const KernelSize = 64;
	static int const HalfKernel = KernelSize / 2;

	float Estimate = 0;
	float Count = 0;
	int NumSamples = 0;

	for (int i = -HalfKernel; i <= HalfKernel; ++ i)
	{
		for (int j = -HalfKernel; j <= HalfKernel; ++ j)
		{
			if (i != 0 || j != 0)
			{
				SPixelBucket * Bucket = Helper_GetBucket(index.X + i, index.Y + j);
				if (Bucket && Bucket->Count)
				{
					float Weight = 1 + Sqrt((float) Sq(HalfKernel - Abs(i)) + (float) Sq(HalfKernel - Abs(j))) * 5.0f;
					Estimate += Bucket->GetValue() * Weight;
					Count += Weight;
					NumSamples ++;
				}
			}
		}
	}

	if (NumSamples > 16)
	{
		SPixelBucket * Bucket = Helper_GetBucket(index);
		Bucket->Count = 1;
		Bucket->Sum = Estimate / (float) Count;
	}
}

void CBathymetryRasterizer::Helper_ReconstructTagGroup(STagInfo & Group, int const Tag)
{
	//
	// A potential better way to do this:
	// Make kernel size a parameter, then run filling for a small kernel without finalizing values
	// Once every pixel is hit, increase kernel size and run again with the new values
	// This makes an accurate approximation for values close to edges, more approximate values towards the center of gaps
	// Also potentially just don't use any approximate values at all for hole filling
	//

	vector<vec2i> Queue = Helper_GetAllMatchingGroup(Tag);
	while (Queue.size())
	{
		for (auto const & Spot : Queue)
		{
			Helper_EstimatePixelValue(Spot);
		}

		for (auto it = Queue.begin(); it != Queue.end(); )
		{
			if (Helper_GetBucket(*it)->Count > 0)
			{
				it = Queue.erase(it);
			}
			else
			{
				it ++;
			}
		}
	}
}

void CBathymetryRasterizer::FillGroups()
{
	CStopWatch sw;
	sw.Start();

	int tag = 1;
	while (TagGroups[tag].count)
	{
		if (TagGroups[tag].count < 16384)
		{
			Helper_ReconstructTagGroup(TagGroups[tag], tag);
		}
		tag ++;
	}

	Log::Info("Fill groups took %.3f", sw.Stop());
}

void CBathymetryRasterizer::RasterizeImage()
{
	CStopWatch sw;
	sw.Start();

	int LowPixel = 255;
	int HighPixel = 0;

	vector<vec2f> Outline = CatalinaOutline;
	std::reverse(Outline.begin(), Outline.end());
	vector<STriangle2D> const Triangles = TriangulateEarClipping(Outline);

	byte * ImageData = new byte[ImageSize * ImageSize * 3];
	for (int i = 0; i < ImageSize; ++ i)
	{
		for (int j = 0; j < ImageSize; ++ j)
		{
			int const Index = i + j * ImageSize;

			if (Buckets[Index].Count > 0)
			{
				float const Value = Buckets[Index].Sum / Buckets[Index].Count;
				float const Intensity = 2.5f;
				float const Bottom = 0;
				int const Pixel = 255 - Clamp<int>((int) (Value * Intensity - Bottom), 0, 255);

				LowPixel = Min(LowPixel, Pixel);
				HighPixel = Max(HighPixel, Pixel);

				ImageData[Index * 3 + 0] = Pixel;
				ImageData[Index * 3 + 1] = 0;
				ImageData[Index * 3 + 2] = 0;
			}
			else
			{
				ImageData[Index * 3 + 0] = 0;
				ImageData[Index * 3 + 1] = 0;
				ImageData[Index * 3 + 2] = 255;
			}

			bool InsideTriangle = false;
			vec2f const Point = vec2f(
				RegionXCorner + ((float) i / (float) (ImageSize - 1)) * RegionXSize,
				RegionYCorner + ((float) j / (float) (ImageSize - 1)) * RegionYSize
			);

			for (auto const & Triangle : Triangles)
			{
				if (ion::IsPointInTriangle(Triangle.A.YX(), Triangle.B.YX(), Triangle.C.YX(), Point))
				{
					InsideTriangle = true;
					break;
				}
			}

			if (InsideTriangle)
			{
				ImageData[Index * 3 + 1] = 255;
			}
		}
	}

	Log::Info("Low value: %d High Value: %d", LowPixel, HighPixel);

	CImage * Image = new CImage(ImageData, vec2u(ImageSize), 3);
	Image->FlipY();
	Image->Write(OutputName);

	Log::Info("Rasterize to image took %.3f", sw.Stop());
}
