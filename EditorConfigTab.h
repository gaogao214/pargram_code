#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
//#include "SBCMN/excel/ExcelParser.h"

namespace sandbox
{
#define CONFIG_HEAD_ROW_NUM 5         //����ͷ������
#define ASSET_PATH "../../../Parsing_the_text/"

	struct EditorConfigRow;
	struct EditorConfigItem      //�༭��������
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

	struct EditorConfigRowRef   //�༭������������
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
	struct EditorConfigRow          //�༭��������    ������
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

	class EditorConfigTab  //�༭������ѡ�   �༭����е�ͷ
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

	class EditorConfigMark  //�༭�����ñ��
	{
	public:
		struct Mark
		{
			int m_snapmode = 1;  //����ģʽ
			float m_snapoffset = .0f;     //ƫ������
			float m_posx = .0f;      //��ʼλ������  x 
			float m_posy = .0f;      //��ʼλ������  y 
			float m_posz = .0f;      //��ʼλ������  z 
			float m_dirw = .0f;      //����  Ŀ¼  �ļ���  w 
			float m_dirx = .0f;		 //����  Ŀ¼  �ļ���  x 
			float m_diry = .0f;		 //����  Ŀ¼  �ļ���  y 
			float m_dirz = .0f;		 //����  Ŀ¼  �ļ���  z 
		};

		struct Data						//������
		{
			float m_posx = .0f;			//��ʼλ������  x
			float m_posy = .0f;			//��ʼλ������  y
			float m_posz = .0f;			//��ʼλ������  z
			uint32_t m_refreshid = 0;   //ˢ��id
			std::vector<Mark> m_marks;  
		};

	public:
		EditorConfigMark();
		EditorConfigMark(const char* file);
		bool Load();
		bool Load(const char* file);
		void Save();
		void Save(const char* file);   //�����γɱ����ļ�

	public:
		std::string m_file{};
		uint32_t m_magicid = 0;
		uint32_t m_version = 0;
		std::vector<Data> m_datas;
	};
}
#ifdef CON_EDITOR

#endif 