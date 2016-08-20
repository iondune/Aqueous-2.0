
#include <ionEngine.h>


vector<double> LoadOxymapsCSVData(string const & FileName)
{
	vector<double> Elements;

	string const FileContents = File::ReadAsString(FileName);
	vector<string> const ElementStrings = String::Explode(FileContents, ',');
	std::transform(ElementStrings.begin(), ElementStrings.end(), std::back_inserter(Elements), [](string const & s) -> double
	{
		return atof(s.c_str());
	});

	double const Max = *std::max_element(Elements.begin(), Elements.end());
	double Min = Max;
	std::for_each(Elements.begin(), Elements.end(), [&Min](double const & Element)
	{
		if (Element != 0)
		{
			Min = Minimum(Element, Min);
		}
	});

	std::for_each(Elements.begin(), Elements.end(), [Max, Min](double & Element)
	{
		Element = Clamp((Element - Min) / (Max - Min), 0.0, 1.0);
	});

	return Elements;
}
