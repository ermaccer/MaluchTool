#include "tmfile.h"
#include "depak.h"
#include <fstream>
#include "framework.h"
#include "filef.h"
#include <filesystem>
#include <memory>
#include <CommCtrl.h>
#include <shlobj.h>
#include "eObj.h"

ETMFile::ETMFile(std::wstring in, std::wstring out, int m)
{
	mode = m;
	inPath = in;
	outPath = out;
}

bool ETMFile::Process()
{
	if (mode == MODE_EXTRACT)
	{
		pFile.open(inPath, std::ifstream::binary);
		SendMessage(*progressBar, PBM_SETPOS, 0, 0);
		SendMessage(*progressBar, PBM_SETSTATE, PBST_NORMAL, 0);

		if (!pFile.is_open())
		{
			MessageBox(0, L"Could not open input file!", 0, MB_ICONWARNING);
			return false;
		}
		if (pFile.is_open())
		{
			tm_header header;

			pFile.read((char*)&header, sizeof(tm_header));

			if (!(strcmp(header.markerType, "model") == 0))
			{
				MessageBox(0, L"Unsupported marker type! Only model is supported.", 0, MB_ICONWARNING);
				return false;
			}

			tm_model_marker marker;
			pFile.read((char*)&marker, sizeof(tm_model_marker));

			if (!std::filesystem::exists(outPath))
				std::filesystem::create_directory(outPath);

			std::filesystem::current_path(outPath);

			// update progress bar
			SendMessage(*progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, marker.meshes));




			for (int i = 0; i < marker.meshes; i++)
			{
				tm_mesh_entry mesh;
				pFile.read((char*)&mesh, sizeof(tm_mesh_entry));

				wsprintf(progressBuffer, L"%d/%d", i + 1, marker.meshes);
				SetWindowText(*filename, progressBuffer);
				SendMessage(*progressBar, PBM_STEPIT, 0, 0);

				int unkAmount = 0;
				pFile.read((char*)&unkAmount, sizeof(int));
				for (int i = 0; i < unkAmount; i++)
				{

					float unk[16];
					pFile.read((char*)&unk, sizeof(unk));

				}

				int faceAmount = 0;
				pFile.read((char*)&faceAmount, sizeof(int));

				std::vector<obj_face> vFaces;

				for (int i = 0; i < faceAmount / 3; i++)
				{
					obj_face face;
					pFile.read((char*)&face, sizeof(obj_face));
					vFaces.push_back(face);
				}

				int vertsAmount = 0;
				pFile.read((char*)&vertsAmount, sizeof(int));
				std::vector<obj_v> vVerts;
				std::vector<obj_uv> vMaps;
				std::vector<obj_vn> vNorm;

				for (int i = 0; i < vertsAmount; i++)
				{
					obj_v v;
					pFile.read((char*)&v, sizeof(obj_v));
					vVerts.push_back(v);
				}

				for (int i = 0; i < vertsAmount; i++)
				{

					obj_vn vn;
					pFile.read((char*)&vn, sizeof(obj_vn));
					vn.norm[0] = 0.0f;
					vNorm.push_back(vn);
				}


				for (int i = 0; i < vertsAmount; i++)
				{
					obj_uv uv;
					pFile.read((char*)&uv, sizeof(obj_uv));
					vMaps.push_back(uv);
				}


				eObj obj;
				obj.SetModelData(vVerts, vMaps, vNorm, vFaces);
				obj.SaveFile(mesh.modelName, mesh.modelName, mesh.textureName);

				std::string name_obj = mesh.modelName;
				name_obj += ".obj";


			}
			return true;
			
		}
	}
	if (mode == MODE_CREATE)
	{
		SendMessage(*progressBar, PBM_SETPOS, 0, 0);
		SendMessage(*progressBar, PBM_SETSTATE, PBST_NORMAL, 0);

		if (!std::filesystem::exists(inPath))
		{
			MessageBox(0, L"Folder does not exist!", 0, 0);
			return false;
		}

		std::vector<eObj> vModels;

		for (const auto & file : std::filesystem::recursive_directory_iterator(inPath))
		{
			if (file.path().has_extension())
			{
				if (file.path().extension().string() == ".obj")
				{
					eObj model;
					std::string cnv;
					std::copy(cnv.begin(), cnv.end(), inPath.begin());
					//model.SetFolder(cnv.c_str());
					model.LoadFile(file.path().string().c_str());
				//	std::filesystem::current_path("..");
					vModels.push_back(model);
				}
			}
		}

		// update progress bar
		SendMessage(*progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, vModels.size()));

		std::ofstream oFile(outPath, std::ofstream::binary);

		tm_header header;
		sprintf(header.markerType, "model");

		oFile.write((char*)&header, sizeof(tm_header));

		tm_model_marker marker;
		marker.meshes = vModels.size();
		oFile.write((char*)&marker, sizeof(tm_model_marker));

		for (int i = 0; i < vModels.size(); i++)
		{
			wsprintf(progressBuffer, L"%d/%d", i + 1, marker.meshes);
			SetWindowText(*filename, progressBuffer);
			SendMessage(*progressBar, PBM_STEPIT, 0, 0);
			tm_mesh_entry mesh;
			sprintf(mesh.modelName, "%s", vModels[i].GetMeshName().c_str());
			sprintf(mesh.textureName, "%s", vModels[i].GetTextureName().c_str());

			oFile.write((char*)&mesh, sizeof(tm_mesh_entry));

			int unkAmount = 0;
			oFile.write((char*)&unkAmount, sizeof(int));


			int faceAmount = vModels[i].GetFaces().size() * 3;
			oFile.write((char*)&faceAmount, sizeof(int));

			for (int a = 0; a < vModels[i].GetFaces().size(); a++)
			{
				int f[3];
				f[0] = vModels[i].GetFaces()[a].face[0];
				f[1] = vModels[i].GetFaces()[a].face[1];
				f[2] = vModels[i].GetFaces()[a].face[2];
				oFile.write((char*)&f, sizeof(f));
			}

			int vertexAmount = vModels[i].GetVertexes().v.size();
			oFile.write((char*)&vertexAmount, sizeof(int));

			for (int a = 0; a < vModels[i].GetVertexes().v.size(); a++)
			{
				oFile.write((char*)&vModels[i].GetVertexes().v[a], sizeof(obj_v));
			}
			for (int a = 0; a < vModels[i].GetVertexes().v.size(); a++)
			{
				oFile.write((char*)&vModels[i].GetVertexes().vn[a], sizeof(obj_vn));
			}
			for (int a = 0; a < vModels[i].GetVertexes().v.size(); a++)
			{
				obj_uv newUV = vModels[i].GetVertexes().uv[a];
				newUV.v = 1.0f - newUV.v;
				oFile.write((char*)&newUV, sizeof(obj_uv));
			}

			//vModels[i].Print();
		}
		return true;
	}
	return false;
}

void ETMFile::AttachProgressBar(HWND * bar)
{
	progressBar = bar;
	SendMessage(*progressBar, PBM_SETSTEP, 1, 0);
}

void ETMFile::AttachFilenameText(HWND * txt)
{
	filename = txt;
}
