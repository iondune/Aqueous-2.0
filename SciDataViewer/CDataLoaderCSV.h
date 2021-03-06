
#pragma once

#include "CDataLoader.h"


class CDataLoaderCSV : public CDataLoader
{

public:

	string FileName;
	char FieldDelim = ',';
	char ValueDelim = ',';

	virtual void Load(CDataSet * DataSet)
	{
		std::ifstream File;
		vector<string> Fields;
		bool FirstLine = true;

		File.open(FileName);
		while (File.is_open() && File.good())
		{
			string Line;
			std::getline(File, Line);

			if (Line == "")
				continue;

			std::istringstream Stream(Line);

			if (FirstLine)
			{
				vector<string> ReadFields;
				while (Stream.good())
				{
					string Label;
					std::getline(Stream, Label, FieldDelim);
					ReadFields.push_back(Label);
				}
				Fields = ReadFields;
				DataSet->SetSchema(Fields);
				FirstLine = false;
			}
			else
			{
				vector<string> Row;
				while (Stream.good())
				{
					string Label;
					std::getline(Stream, Label, ValueDelim);
					Row.push_back(Label);
				}

				//if (Row.size() != Fields.size())
				//	std::cerr << "Mismatched row size at row " << DataSet->Size() << " in file '" << FileName << "', found " << Row.size() << " but expected " << Fields.size() << std::endl;

				DataSet->AddRow(Row);
			}
		}
	}

};
