#pragma once

//Include guards because this is just an archive. Operations and sequences pre-CGIL4 have a different format so all of these functions will throw errors
#ifdef SDFSfersfRSDFGS
Project* LoadCGIL0(std::ifstream& InputFile)
{
	int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
	InputFile.read((char*)&IntVal, sizeof(int));
	InputFile.read((char*)&IntVal, sizeof(int));
	InputFile.read((char*)&IntVal, sizeof(int));

	Project* Proj = new Project();
	LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

	int Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the operations
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Operations[Name] = new DynamicOperation();
		Proj->Operations[Name]->Load(InputFile);
	}

	Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Sequences[Name] = new StaticSequence();
		Proj->Sequences[Name]->Load(InputFile);
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));    //Load the entry point
	Proj->Main.reserve(Len);
	for (int i = 0; i < Len; ++i)
	{
		Proj->Main.push_back(Token::Load(InputFile));
	}
	return Proj;
}

Project* LoadCGIL1(std::ifstream& InputFile)
{
	int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
	InputFile.read((char*)&IntVal, sizeof(int));
	InputFile.read((char*)&IntVal, sizeof(int));
	InputFile.read((char*)&IntVal, sizeof(int));

	Project* Proj = new Project();
	LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

	int Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the operations
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Operations[Name] = new DynamicOperation();
		Proj->Operations[Name]->Load(InputFile);
	}

	Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Sequences[Name] = new StaticSequence();
		Proj->Sequences[Name]->Load(InputFile);
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));    //Load the entry point
	Proj->Main.reserve(Len);
	for (int i = 0; i < Len; ++i)
	{
		Proj->Main.push_back(Token::Load(InputFile));
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));    //Load the imports
	Proj->Imports.reserve(Len);
	for (int i = 0; i < Len; ++i)
	{
		std::string buff;
		LoadStringFromFile(buff, InputFile);
		Proj->Imports.push_back(std::move(buff));
	}
	return Proj;
}

Project* LoadCGIL2(std::ifstream& InputFile)
{
	int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
	InputFile.read((char*)&IntVal, sizeof(int));
	InputFile.read((char*)&IntVal, sizeof(int));
	InputFile.read((char*)&IntVal, sizeof(int));

	Project* Proj = new Project();
	LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

	int Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the operations
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Operations[Name] = Operation::LoadOperation(InputFile);
	}

	Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Sequences[Name] = Sequence::LoadSequence(InputFile);
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));    //Load the entry point
	Proj->Main.reserve(Len);
	for (int i = 0; i < Len; ++i)
	{
		Proj->Main.push_back(Token::Load(InputFile));
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));    //Load the imports
	Proj->Imports.reserve(Len);
	for (int i = 0; i < Len; ++i)
	{
		std::string buff;
		LoadStringFromFile(buff, InputFile);
		Proj->Imports.push_back(std::move(buff));
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));    //Load the namespaces
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Namespaces[Name] = Project::Load(InputFile, Name);
	}
	return Proj;
}

Project* LoadCGIL3(std::ifstream& InputFile)
{
	int IntVal;    //GIL versions are only for debugging, so read them into a useless buffer
	InputFile.read((char*)&IntVal, sizeof(int));
	InputFile.read((char*)&IntVal, sizeof(int));
	InputFile.read((char*)&IntVal, sizeof(int));

	Project* Proj = new Project();
	LoadStringFromFile(Proj->TargetOrganism, InputFile);    //Load the target organism

	int Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the operations
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Operations[Name] = Operation::LoadOperation(InputFile);
	}

	Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the sequences
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Sequences[Name] = Sequence::LoadSequence(InputFile);
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));    //Load the entry point
	Proj->Main.reserve(Len);
	for (int i = 0; i < Len; ++i)
	{
		Proj->Main.push_back(Token::Load(InputFile));
	}

	Len = -1;
	InputFile.read((char*)&Len, sizeof(int));    //Load the imports
	Proj->Imports.reserve(Len);
	for (int i = 0; i < Len; ++i)
	{
		std::string buff;
		LoadStringFromFile(buff, InputFile);
		Proj->Imports.push_back(std::move(buff));
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));    //Load the namespaces
	for (int i = 0; i < Len; ++i)
	{
		std::string Name;
		LoadStringFromFile(Name, InputFile);
		Proj->Namespaces[Name] = Project::Load(InputFile, Name);
	}

	Len = 0;
	InputFile.read((char*)&Len, sizeof(int));
	Proj->Usings.reserve(Len);
	for (int i = 0; i < Len; ++i)
	{
		std::string buff;
		LoadStringFromFile(buff, InputFile);
		Proj->Usings.push_back(std::move(buff));
	}
	return Proj;
}
#endif