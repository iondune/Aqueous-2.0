
#include "CBathymetryRasterizer.h"
#include "CatalinaOutline.h"

#include <ionScience.h>

using namespace ion;


CBathymetryRasterizer::CBathymetryRasterizer()
{
	Buckets = new SPixelBucket[ImageSize * ImageSize];
}

void CBathymetryRasterizer::ConvertAndRasterize(bool const Processing)
{
	CopySourcePointsToBuckets();
	if (Processing)
	{
		ClassifyGroups();
		DetectBridgeGroups();
		FillBridgeGroups();
		FillGroups();
	}
	RasterizeImage();
}

bool CBathymetryRasterizer::IsPointInBounds(vec2d const & Position)
{
	if ((Position.X >= RegionXCorner && Position.X <= RegionXCorner + RegionXSize) &&
		(Position.Y >= RegionYCorner && Position.Y <= RegionYCorner + RegionYSize))
	{
		vec2d RealIndex = vec2d(
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

double CBathymetryRasterizer::GetHeightAtPoint(vec2d const & Position)
{
	vec2d const RealIndex = vec2d(
		(Position.X - RegionXCorner) / RegionXSize,
		(Position.Y - RegionYCorner) / RegionYSize) * (double) ImageSize;

	int const i = Clamp((int) RealIndex.X, 0, ImageSize - 1);
	int const j = Clamp((int) RealIndex.Y, 0, ImageSize - 1);

	auto Bucket = Helper_GetBucket(i, j);
	if (Bucket && Bucket->Count)
	{
		double const Value = Bucket->GetValue();

		double Data[4][4] =
		{
			{ Value, Value, Value, Value },
			{ Value, Value, Value, Value },
			{ Value, Value, Value, Value },
			{ Value, Value, Value, Value },
		};

		for (int m = 0; m < 4; ++ m)
		{
			for (int n = 0; n < 4; ++ n)
			{
				auto Bucket = Helper_GetBucket(i + m - 1, j + n - 1);
				if (Bucket && Bucket->Count)
				{
					Data[m][n] = Bucket->GetValue();
				}
			}
		}

		double integral;
		return BicubicInterpolate(Data, modf(RealIndex.X, &integral), modf(RealIndex.Y, &integral));
	}
	else
	{
		return std::numeric_limits<double>::quiet_NaN();
	}

}

void CBathymetryRasterizer::WriteToFile(string const & FileName)
{
	FILE * file = nullptr;
	file = fopen(FileName.c_str(), "wb");

	fwrite(& ImageSize, sizeof(ImageSize), 1, file);
	fwrite(& RegionXCorner, sizeof(RegionXCorner), 1, file);
	fwrite(& RegionYCorner, sizeof(RegionYCorner), 1, file);
	fwrite(& RegionXSize, sizeof(RegionXSize), 1, file);
	fwrite(& RegionYSize, sizeof(RegionYSize), 1, file);

	fwrite(Buckets, sizeof(SPixelBucket), ImageSize * ImageSize, file);

	fclose(file);
}

void CBathymetryRasterizer::ReadFromFile(string const & FileName)
{
	FILE * file = nullptr;
	file = fopen(FileName.c_str(), "rb");

	fread(& ImageSize, sizeof(ImageSize), 1, file);
	fread(& RegionXCorner, sizeof(RegionXCorner), 1, file);
	fread(& RegionYCorner, sizeof(RegionYCorner), 1, file);
	fread(& RegionXSize, sizeof(RegionXSize), 1, file);
	fread(& RegionYSize, sizeof(RegionYSize), 1, file);

	fread(Buckets, sizeof(SPixelBucket), ImageSize * ImageSize, file);

	fclose(file);
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

	vector<STriangle2D<double>> Triangles;
	AddAtEnd(Triangles, TriangulateEarClipping(CatalinaOutline));
	AddAtEnd(Triangles, TriangulateEarClipping(BirdRock));

	for (int i = 0; i < ImageSize; ++ i)
	{
		for (int j = 0; j < ImageSize; ++ j)
		{
			bool InsideTriangle = false;
			vec2d const Point = vec2d(
				RegionXCorner + ((double) i / (double) (ImageSize - 1)) * RegionXSize,
				RegionYCorner + ((double) j / (double) (ImageSize - 1)) * RegionYSize
			);

			for (auto const & Triangle : Triangles)
			{
				if (ion::IsPointInOrOnTriangle(Triangle.A, Triangle.C, Triangle.B, Point))
				{
					InsideTriangle = true;
					break;
				}
			}

			if (InsideTriangle)
			{
				auto Bucket = Helper_GetBucket(i, j);
				Bucket->Tag = -1;
				Bucket->Count = 1;
				Bucket->Sum = 0;
			}
		}
	}

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

void CBathymetryRasterizer::DetectBridgeGroups()
{
	CStopWatch sw;
	sw.Start();

	static vector<vec2i> const Neighbors =
	{
		vec2i(-1, 0),
		vec2i(1, 0),
		vec2i(0, -1),
		vec2i(0, 1),
	};

	int Tag = 1;
	while (TagGroups[Tag].count)
	{
		vector<vec2i> const Group = Helper_GetAllMatchingGroup(Tag);

		bool NextToData = false;
		bool NextToLand = false;
		vec2i LandPt, DataPt;

		for (auto Tile : Group)
		{
			for (auto Neighbor : Neighbors)
			{
				auto Bucket = Helper_GetBucket(Tile + Neighbor);

				if (Bucket)
				{
					if (Bucket->Count > 0)
					{
						NextToData = true;
						DataPt = Tile;
						if (NextToLand)
						{
							break;
						}
					}

					if (Bucket->Tag == -1)
					{
						NextToLand = true;
						LandPt = Tile;
						if (NextToData)
						{
							break;
						}
					}
				}
			}

			if (NextToData && NextToLand)
			{
				break;
			}
		}

		if (NextToData && NextToLand)
		{
			TagGroups[Tag].IsBridge = true;
			Log::Info("      Tag group %d is bridge because {%d, %d} next to Data and {%d, %d} next to Land", Tag, DataPt.X, DataPt.Y, LandPt.X, LandPt.Y);
		}

		Tag ++;
	}

	Log::Info("Detect bridge groups took %.3f", sw.Stop());
}

void CBathymetryRasterizer::FillBridgeGroups()
{
	CStopWatch sw;
	sw.Start();

	static vector<vec2i> const Neighbors =
	{
		vec2i(-1, 0),
		vec2i(1, 0),
		vec2i(0, -1),
		vec2i(0, 1),
	};

	int Tag = 1;
	while (TagGroups[Tag].count)
	{
		if (TagGroups[Tag].IsBridge)
		{
			vector<vec2i> Queue = Helper_GetAllMatchingGroup(Tag);

			int KernelSize = 4;
			int MinSamples = 2;

			while (Queue.size() && KernelSize <= 512)
			{
				for (auto const & Spot : Queue)
				{
					Helper_EstimatePixelValueBridge(Spot, KernelSize, MinSamples);
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
				
				KernelSize += 2;
				MinSamples = 4;
			}
		}

		Tag ++;
	}

	Log::Info("Fill bridge groups took %.3f", sw.Stop());
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

void CBathymetryRasterizer::Helper_EstimatePixelValue(vec2i const & index, int const KernelSize)
{
	int const HalfKernel = KernelSize / 2;

	double Estimate = 0;
	double Count = 0;
	int NumSamples = 0;

	for (int i = -HalfKernel; i <= HalfKernel; ++ i)
	{
		for (int j = -HalfKernel; j <= HalfKernel; ++ j)
		{
			if (i != 0 || j != 0)
			{
				SPixelBucket * Bucket = Helper_GetBucket(index.X + i, index.Y + j);
				if (Bucket && Bucket->Count && ! Bucket->Approximate)
				{
					double Weight = 1 + Sqrt((double) Sq(HalfKernel - Abs(i)) + (double) Sq(HalfKernel - Abs(j))) * 5.0f;
					Estimate += Bucket->GetValue() * Weight;
					Count += Weight;
					NumSamples ++;
				}
			}
		}
	}

	if (NumSamples > 4)
	{
		SPixelBucket * Bucket = Helper_GetBucket(index);
		Bucket->Count = 1;
		Bucket->Sum = Estimate / (double) Count;
		Bucket->Approximate = true;
	}
}

void CBathymetryRasterizer::Helper_EstimatePixelValueBridge(vec2i const & index, int const KernelSize, int const MinSamples)
{
	static vector<pair<double, double>> BathyEstimates;
	static vector<double> LandDistances;

	BathyEstimates.clear();
	LandDistances.clear();

	int const HalfKernel = KernelSize / 2;

	for (int i = -HalfKernel; i <= HalfKernel; ++ i)
	{
		for (int j = -HalfKernel; j <= HalfKernel; ++ j)
		{
			if (i != 0 || j != 0)
			{
				double const Distance = (double) Sq(i) + (double) Sq(j);

				SPixelBucket * Bucket = Helper_GetBucket(index.X + i, index.Y + j);

				if (Bucket && Bucket->Count && ! Bucket->Approximate && Bucket->Tag == 0)
				{
					BathyEstimates.push_back(make_pair(Distance, Bucket->GetValue()));
				}

				if (Bucket && Bucket->Tag == -1)
				{
					LandDistances.push_back(Distance);
				}
			}
		}
	}

	if ((int) BathyEstimates.size() >= MinSamples && (int) LandDistances.size() >= MinSamples)
	{
		std::sort(BathyEstimates.begin(), BathyEstimates.end());
		std::sort(LandDistances.begin(), LandDistances.end());

		BathyEstimates.resize(MinSamples);
		LandDistances.resize(MinSamples);

		double Weight = 0;
		double Sum = 0;

		for (auto Estimate : BathyEstimates)
		{
			double const w = 1.f / Sqrt(Estimate.first);
			Weight += w;
			Sum += Estimate.second * w;
		}

		for (auto Distance : LandDistances)
		{
			Weight += 1.f / Sqrt(Distance);
		}

		SPixelBucket * Bucket = Helper_GetBucket(index);
		Bucket->Count = 1;
		Bucket->Sum = Sum / Weight;
		Bucket->Approximate = true;
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

	int KernelSize = 4;

	vector<vec2i> Queue = Helper_GetAllMatchingGroup(Tag);
	while (Queue.size() && KernelSize <= 512)
	{
		for (auto const & Spot : Queue)
		{
			Helper_EstimatePixelValue(Spot, KernelSize);
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

		//if (KernelSize < 32)
		//{
			KernelSize *= 2;
		//}
		//else if (KernelSize < 64)
		//{
		//	KernelSize += 4;
		//}
		//else
		//{
		//	KernelSize += 64;
		//}
	}
}

void CBathymetryRasterizer::FillGroups()
{
	CStopWatch sw;
	sw.Start();

	int tag = 1;
	while (TagGroups[tag].count)
	{
		if (TagGroups[tag].count < 96 && ! TagGroups[tag].IsBridge)
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

	byte * ImageData = new byte[ImageSize * ImageSize * 3];
	for (int i = 0; i < ImageSize; ++ i)
	{
		for (int j = 0; j < ImageSize; ++ j)
		{
			int const Index = ImageSize * i + j;

			if (Buckets[Index].Count > 0)
			{
				double const Value = Buckets[Index].Sum / Buckets[Index].Count;
				double const Intensity = 2.5f;
				double const Bottom = 0;
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

			if (Buckets[Index].Tag == -1)
			{
				ImageData[Index * 3 + 1] = 255;
			}
			//else if (Buckets[Index].Tag > 0)
			//{
			//	if (TagGroups[Buckets[Index].Tag].IsBridge)
			//	{
			//		ImageData[Index * 3 + 1] = ImageData[Index * 3 + 0];
			//		ImageData[Index * 3 + 0] = 0;
			//		ImageData[Index * 3 + 2] = 128;
			//	}
			//}
		}
	}

	Log::Info("Low value: %d High Value: %d", LowPixel, HighPixel);

	CImage * Image = new CImage(ImageData, vec2u(ImageSize), 3);
	Image->FlipY();
	Image->Write(OutputName);

	Log::Info("Rasterize to image took %.3f", sw.Stop());
}

double LineSegmentToPointDistance(vec2d const & v, vec2d const & w, vec2d const & p)
{
	// Return minimum distance between line segment vw and point p
	const double l2 = LengthSq(v - w);  // i.e. |w-v|^2 -  avoid a sqrt
	if (l2 == 0.0) return Length(p - v);   // v == w case
											// Consider the line extending the segment, parameterized as v + t (w - v).
											// We find projection of point p onto the line. 
											// It falls where t = [(p-v) . (w-v)] / |w-v|^2
											// We clamp t from [0,1] to handle points outside the segment vw.
	const double t = Max(0.0, Min(1.0, Dot(p - v, w - v) / l2));
	vec2d const projection = v + t * (w - v);  // Projection falls on the segment
	return Length(p - projection);
}

void CTopographyRasterizer::FillInteriorPoints()
{
	CStopWatch sw;
	sw.Start();

	Buckets = new SPixelBucket[ImageSize * ImageSize];

	vector<STriangle2D<double>> Triangles;
	AddAtEnd(Triangles, TriangulateEarClipping(CatalinaOutline));

	double ActualMax = 0;

	for (int i = 0; i < ImageSize; ++ i)
	{
		for (int j = 0; j < ImageSize; ++ j)
		{
			bool InsideTriangle = false;
			vec2d const Point = vec2d(
				RegionXCorner + ((double) i / (double) (ImageSize - 1)) * RegionXSize,
				RegionYCorner + ((double) j / (double) (ImageSize - 1)) * RegionYSize
			);

			for (auto const & Triangle : Triangles)
			{
				if (ion::IsPointInOrOnTriangle(Triangle.A, Triangle.C, Triangle.B, Point))
				{
					InsideTriangle = true;
					break;
				}
			}

			if (InsideTriangle)
			{
				auto Bucket = Helper_GetBucket(i, j);
				Bucket->Interior = true;
				Bucket->Value = Helper_ClosestEdgeDistance(Point);

				ActualMax = Max(ActualMax, Bucket->Value);
			}
		}
	}

	double const ApexPoint = 0.06;
	double const ScaleUp = 255.0 / 0.05;

	for (int i = 0; i < ImageSize; ++ i)
	{
		for (int j = 0; j < ImageSize; ++ j)
		{
			auto Bucket = Helper_GetBucket(i, j);
			Bucket->Value /= ApexPoint;
			Bucket->Value = pow(Bucket->Value, 0.65f);
			Bucket->Value *= ApexPoint * ScaleUp;
		}
	}

	Log::Info("Max: %f Scaled Max: %f", ActualMax, ActualMax * ScaleUp);
	Log::Info("Topography fill interior took %.3f", sw.Stop());
}

void CTopographyRasterizer::RasterizeImage()
{
	CStopWatch sw;
	sw.Start();

	int LowPixel = 255;
	int HighPixel = 0;

	byte * ImageData = new byte[ImageSize * ImageSize * 3];
	for (int i = 0; i < ImageSize; ++ i)
	{
		for (int j = 0; j < ImageSize; ++ j)
		{
			int const Index = ImageSize * i + j;

			double const Value = Buckets[Index].Value;
			double const Intensity = 1.0f;
			int const Pixel = Clamp<int>((int) (Value * Intensity), 0, 255);

			LowPixel = Min(LowPixel, Pixel);
			HighPixel = Max(HighPixel, Pixel);

			ImageData[Index * 3 + 0] = Pixel;
			ImageData[Index * 3 + 1] = Pixel;
			ImageData[Index * 3 + 2] = Pixel;
			
		}
	}

	Log::Info("Low value: %d High Value: %d", LowPixel, HighPixel);

	CImage * Image = new CImage(ImageData, vec2u(ImageSize), 3);
	Image->FlipY();
	Image->Write(OutputName);

	Log::Info("Rasterize to image took %.3f", sw.Stop());
}

void CTopographyRasterizer::WriteToFile(string const & FileName)
{
	FILE * file = nullptr;
	file = fopen(FileName.c_str(), "wb");

	fwrite(& ImageSize, sizeof(ImageSize), 1, file);
	fwrite(& RegionXCorner, sizeof(RegionXCorner), 1, file);
	fwrite(& RegionYCorner, sizeof(RegionYCorner), 1, file);
	fwrite(& RegionXSize, sizeof(RegionXSize), 1, file);
	fwrite(& RegionYSize, sizeof(RegionYSize), 1, file);

	fwrite(Buckets, sizeof(SPixelBucket), ImageSize * ImageSize, file);

	fclose(file);
}

void CTopographyRasterizer::ReadFromFile(string const & FileName)
{
	FILE * file = nullptr;
	file = fopen(FileName.c_str(), "rb");

	fread(& ImageSize, sizeof(ImageSize), 1, file);
	fread(& RegionXCorner, sizeof(RegionXCorner), 1, file);
	fread(& RegionYCorner, sizeof(RegionYCorner), 1, file);
	fread(& RegionXSize, sizeof(RegionXSize), 1, file);
	fread(& RegionYSize, sizeof(RegionYSize), 1, file);

	Buckets = new SPixelBucket[ImageSize * ImageSize];
	fread(Buckets, sizeof(SPixelBucket), ImageSize * ImageSize, file);

	fclose(file);
}

bool CTopographyRasterizer::IsPointInBounds(vec2d const & Position)
{
	double const Margin = 0.0001;
	if ((Position.X > RegionXCorner + Margin && Position.X < RegionXCorner + RegionXSize - Margin) &&
		(Position.Y > RegionYCorner + Margin && Position.Y < RegionYCorner + RegionYSize - Margin))
	{
		vec2d RealIndex = vec2d(
			(Position.X - RegionXCorner) / RegionXSize,
			(Position.Y - RegionYCorner) / RegionYSize) * (double) ImageSize;

		int i = (int) RealIndex.X;
		int j = (int) RealIndex.Y;

		auto Bucket = Helper_GetBucket(i, j);

		if (Bucket && Bucket->Interior)
		{
			return true;
		}
	}

	return false;
}

double CTopographyRasterizer::GetHeightAtPoint(vec2d const & Position)
{
	vec2d const RealIndex = vec2d(
		(Position.X - RegionXCorner) / RegionXSize,
		(Position.Y - RegionYCorner) / RegionYSize) * (double) ImageSize;

	int const i = (int) RealIndex.X;
	int const j = (int) RealIndex.Y;

	auto Bucket = Helper_GetBucket(i, j);
	if (Bucket && Bucket->Interior)
	{
		double const Value = Bucket->Value;

		double Data[4][4] =
		{
			{ Value, Value, Value, Value },
			{ Value, Value, Value, Value },
			{ Value, Value, Value, Value },
			{ Value, Value, Value, Value },
		};

		for (int m = 0; m < 4; ++ m)
		{
			for (int n = 0; n < 4; ++ n)
			{
				auto Bucket = Helper_GetBucket(i + m - 1, j + n - 1);
				if (Bucket && Bucket->Interior)
				{
					Data[m][n] = Bucket->Value;
				}
				else
				{
					Data[m][n] = 0;
				}
			}
		}

		double integral;
		return -BicubicInterpolate(Data, modf(RealIndex.X, &integral), modf(RealIndex.Y, &integral));
	}
	else
	{
		return std::numeric_limits<double>::quiet_NaN();
	}
}

CTopographyRasterizer::SPixelBucket * CTopographyRasterizer::Helper_GetBucket(int const i, int const j)
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

CTopographyRasterizer::SPixelBucket * CTopographyRasterizer::Helper_GetBucket(vec2i const & index)
{
	return Helper_GetBucket(index.X, index.Y);
}

double CTopographyRasterizer::Helper_ClosestEdgeDistance(vec2d const & Point)
{
	double MinDistance = NumericLimits<double>::max();

	for (size_t i = 0; i < SourceLongLatPostings.size(); ++ i)
	{
		vec2d const v = SourceLongLatPostings[i];
		vec2d const w = SourceLongLatPostings[(i + 1) % SourceLongLatPostings.size()];

		double const Distance = LineSegmentToPointDistance(v, w, Point);

		MinDistance = Min(MinDistance, Distance);
	}

	return MinDistance;
}
