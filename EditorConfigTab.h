#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
//#include "SBCMN/excel/ExcelParser.h"

namespace sandbox
{
#define CONFIG_HEAD_ROW_NUM 5         //配置头部行数
#define ASSET_PATH "../../../Parsing_the_text/"

	struct EditorConfigRow;
	struct EditorConfigItem      //编辑或配置项
	{
	public:
		EditorConfigItem(EditorConfigRow& row, std::string& val);
		EditorConfigItem(const EditorConfigItem& val) = default;
		EditorConfigItem(EditorConfigItem&& val) noexcept = default;
		EditorConfigItem& operator=(const EditorConfigItem& val) = delete;
		EditorConfigItem& operator=(EditorConfigItem&& val) noexcept = delete;
		operator std::string() const;
		EditorConfigItem& operator=(const std::string& item);
		EditorConfigItem& operator=(std::string&& item);
		bool operator==(const EditorConfigItem& val) const noexcept;
		bool operator!=(const EditorConfigItem& val) const noexcept;
		bool operator==(const std::string& item) const noexcept;
		bool operator!=(const std::string& item) const noexcept;
		bool operator==(std::nullptr_t) const noexcept;
		bool operator!=(std::nullptr_t) const noexcept;
		bool empty() const;
		const char* c_str() const;

	private:
		EditorConfigRow& m_row;
		std::string& m_data;
	};

	struct EditorConfigRowRef   //编辑或配置行引用
	{
	public:
		EditorConfigRowRef();
		void Incref();
		void Decref();
		void Clear();
		void Resize(int size);
		int Size() const;
		bool Empty() const;
		void Push(const std::string& s);
		int GetID() const;
		std::string& operator[](const int index);
		const std::string& operator[](const int index) const;

	private:
		std::vector<std::string> m_data;
		int m_uses;
	};

	class EditorConfigTab;
	struct EditorConfigRow          //编辑或配置行    配置行
	{
	public:
		EditorConfigRow();
		EditorConfigRow(EditorConfigTab* tab);
		~EditorConfigRow();
		EditorConfigRow(const EditorConfigRow& row);
		EditorConfigRow(EditorConfigRow&& row) noexcept;
		EditorConfigRow& operator=(const EditorConfigRow& row);
		EditorConfigRow& operator=(EditorConfigRow&& row) noexcept;
		void Push(const std::string& s) const;
		void Clear() const;
		void Resize(int size) const;
		int Size() const;
		bool Empty() const;
		int GetID() const;
		void Modify() const;
		bool Valid() const;
		EditorConfigItem operator[](const int index);
		const std::string& operator[](const int index) const;

	private:
		void Incref() const;
		void Decref() const;

	private:
		EditorConfigTab* m_tab = nullptr;
		EditorConfigRowRef* m_ref = nullptr;
	};

	class EditorConfigTab  //编辑或配置选项卡   编辑表格中的头
	{
	public:
		EditorConfigTab();
		EditorConfigTab(const char* file);
		const char* GetFileName() const;
		bool Load();
		bool Load(const std::string& file);
		bool Load(const char* file);
		void Save(bool force = false);
		void Save(const char* file, bool force = false);
		void Reset();
		void Clear();
		void ClearContent();
		void Modify();
		bool IsModify() const;
		EditorConfigRow Find(int id);
		void Insert(const EditorConfigRow& row);
		void Replace(const EditorConfigRow& row);
		void Remove(int id);

	public:
		bool m_modify = false;
		std::string m_file{};
		std::vector<EditorConfigRow> m_head{};
		std::map<int, EditorConfigRow> m_content{};
		std::vector<EditorConfigRow> m_all_content{};
	};

	class EditorConfigMark  //编辑或配置标记
	{
	public:
		struct Mark
		{
			int m_snapmode = 1;  //快速模式
			float m_snapoffset = .0f;     //偏移锁定
			float m_posx = .0f;      //起始位置坐标  x 
			float m_posy = .0f;      //起始位置坐标  y 
			float m_posz = .0f;      //起始位置坐标  z 
			float m_dirw = .0f;      //方向  目录  文件名  w 
			float m_dirx = .0f;		 //方向  目录  文件名  x 
			float m_diry = .0f;		 //方向  目录  文件名  y 
			float m_dirz = .0f;		 //方向  目录  文件名  z 
		};

		struct Data						//数据区
		{
			float m_posx = .0f;			//起始位置坐标  x
			float m_posy = .0f;			//起始位置坐标  y
			float m_posz = .0f;			//起始位置坐标  z
			uint32_t m_refreshid = 0;   //刷新id
			std::vector<Mark> m_marks;  
		};

	public:
		EditorConfigMark();
		EditorConfigMark(const char* file);
		bool Load();
		bool Load(const char* file);
		void Save();
		void Save(const char* file);   //保存形成表后的文件

	public:
		std::string m_file{};
		uint32_t m_magicid = 0;
		uint32_t m_version = 0;
		std::vector<Data> m_datas;
	};
}
#ifdef CON_EDITOR

#endif 