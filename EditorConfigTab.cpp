
//#include "SandboxPch.h"
#include <Assert.h>
#include "EditorConfigTab.h"
#include "ExcelParser.h"
//#include "sys/loader.h"

using namespace std;
namespace sandbox
{
	EditorConfigItem::EditorConfigItem(EditorConfigRow& row, std::string& val)
		: m_row(row)
	    , m_data(val)
	{
	}

	EditorConfigItem::operator std::string() const
	{
		return m_data;
	}

	EditorConfigItem& EditorConfigItem::operator=(const std::string& val)
	{
		m_data = val;
		m_row.Modify();
		return *this;
	}

	EditorConfigItem& EditorConfigItem::operator=(std::string&& val)
	{
		m_data = val;
		m_row.Modify();
		return *this;
	}

	bool EditorConfigItem::operator==(const EditorConfigItem& item) const noexcept
	{
		return m_data == item.m_data;
	}

	bool EditorConfigItem::operator!=(const EditorConfigItem& item) const noexcept
	{
		return m_data != item.m_data;
	}

	bool EditorConfigItem::operator==(const std::string& val) const noexcept
	{
		return m_data == val;
	}

	bool EditorConfigItem::operator!=(const std::string& val) const noexcept
	{
		return m_data != val;
	}

	bool EditorConfigItem::operator==(std::nullptr_t) const noexcept
	{
		return empty();
	}

	bool EditorConfigItem::operator!=(std::nullptr_t) const noexcept
	{
		return !empty();
	}

	bool EditorConfigItem::empty() const
	{
		return m_data.empty();
	}

	const char* EditorConfigItem::c_str() const
	{
		return m_data.c_str();
	}

//---------------------------------------------------------------------------------------------
	EditorConfigRowRef::EditorConfigRowRef()
		: m_uses(1)
	{
	}

	void EditorConfigRowRef::Incref()
	{
		++m_uses;
	}

	void EditorConfigRowRef::Decref()
	{
		if (--m_uses == 0){
			delete this;
		}
	}

	void EditorConfigRowRef::Clear()
	{
		m_data.clear();
	}

	void EditorConfigRowRef::Resize(int size)
	{
		m_data.resize(size);
	}

	int EditorConfigRowRef::Size() const
	{
		return (int)m_data.size();
	}

	bool EditorConfigRowRef::Empty() const
	{
		return m_data.empty();
	}

	void EditorConfigRowRef::Push(const std::string& s)
	{
		m_data.push_back(s);
	}

	int EditorConfigRowRef::GetID() const
	{
		static const size_t ID_COL = 2;
		return atoi(m_data[ID_COL].c_str());
	}

	std::string& EditorConfigRowRef::operator[](const int index)
	{
		return m_data[index];
	}

	const std::string& EditorConfigRowRef::operator[](const int index) const
	{
		return m_data[index];
	}

//---------------------------------------------------------------------------------------------
	EditorConfigRow::EditorConfigRow()
		: m_ref(new EditorConfigRowRef())
	{	
	}

	EditorConfigRow::EditorConfigRow(EditorConfigTab* tab)
		: m_tab(tab)
		, m_ref(new EditorConfigRowRef())
	{
	}

	EditorConfigRow::~EditorConfigRow()
	{
		if (m_ref) {
			m_ref->Decref();
		}
	}

	EditorConfigRow::EditorConfigRow(const EditorConfigRow& row)
	{
		row.Incref();
		m_tab = row.m_tab;
		m_ref = row.m_ref;
	}

	EditorConfigRow::EditorConfigRow(EditorConfigRow&& row) noexcept
	{
		m_tab = row.m_tab;
		m_ref = row.m_ref;
		row.m_ref = nullptr;
	}

	EditorConfigRow& EditorConfigRow::operator=(const EditorConfigRow& row)
	{
		Decref();
		row.Incref();
		m_tab = row.m_tab;
		m_ref = row.m_ref;
		return *this;
	}

	EditorConfigRow& EditorConfigRow::operator=(EditorConfigRow&& row) noexcept
	{
		Decref();
		m_tab = row.m_tab;
		m_ref = row.m_ref;
		row.m_ref = nullptr;
		return *this;
	}

	void EditorConfigRow::Push(const std::string& s) const
	{
		if (m_ref) {
			m_ref->Push(s);
		}
	}

	void EditorConfigRow::Clear() const
	{
		if (m_ref) {
			m_ref->Clear();
		}
	}

	void EditorConfigRow::Resize(int size) const
	{
		if (m_ref) {
			m_ref->Resize(size);
		}
	}

	int EditorConfigRow::Size() const
	{
		if (m_ref) {
			return m_ref->Size();
		}else{
			return 0;
		}
	}

	bool EditorConfigRow::Empty() const
	{
		if (m_ref) {
			return m_ref->Empty();
		}
		else {
			return true;
		}
	}

	int EditorConfigRow::GetID() const
	{
		if (m_ref) {
			return m_ref->GetID();
		}
		else {
			return 0;
		}
	}

	void EditorConfigRow::Modify() const
	{
		if (m_tab) {
			m_tab->Modify();
		}
	}

	bool EditorConfigRow::Valid() const
	{
		return m_tab != nullptr;
	}

	EditorConfigItem EditorConfigRow::operator[](const int index)
	{
		assert(m_ref);
		return EditorConfigItem(*this, (*m_ref)[index]);
	}

	const std::string& EditorConfigRow::operator[](const int index) const
	{
		assert(m_ref);
		return (*m_ref)[index];
	}

	void EditorConfigRow::Incref() const
	{
		if (m_ref) {
			m_ref->Incref();
		}
	}

	void EditorConfigRow::Decref() const
	{
		if (m_ref) {
			m_ref->Decref();
		}
	}

//---------------------------------------------------------------------------------------------
	EditorConfigTab::EditorConfigTab()
		: m_file("")
	{		
	}

	EditorConfigTab::EditorConfigTab(const char* file)
		: m_file(file)
	{	
	}

	const char* EditorConfigTab::GetFileName() const
	{
		return m_file.c_str();
	}

	bool EditorConfigTab::Load()
	{
		return Load(m_file.c_str());
	}

	bool EditorConfigTab::Load(const std::string& file)
	{
		return Load(file.c_str());
	}

	bool EditorConfigTab::Load(const char* file)
	{
		if (file == nullptr || file[0] == '\0') {
			printf(u8"EditorConfigTab::Load 表格文件名不能为空", file);
			return false;
		}
		m_file = file;

		/*IGxBlob* buff = sandbox::GetArchivePack()->GetBlob(file);
		if(!buff){
			CERROR(u8"EditorConfigTab::Load 表格文件%s不存在", file);
			return false;
		}*/
		ifstream buff(file);
		if (!buff)
		{
			cout << "文件打开失败\n";
			return false;
		}
		
		excel::excel_parser ep;
		if (!ep.load(file)){
			printf(u8"EditorConfigTab::Load 表格%s的编码格式错误，只支持UTF-8!", file);
			delete file;
			return false;
		}
		//delete file;   

		Reset();

		int count = 0;
		excel::excel_parser::line_data* row_data = ep.m_lines.m_head;
		while(row_data) {
			if (count < CONFIG_HEAD_ROW_NUM) {
				EditorConfigRow row{ this };
				excel::excel_parser::text_data* col_data = row_data->m_texts.m_head;
				while (col_data) {
					row.Push(col_data->m_text);
					col_data = col_data->m_next;
				}
				m_head.emplace_back(std::move(row));
			} else {
				EditorConfigRow row{this};
				excel::excel_parser::text_data* col_data = row_data->m_texts.m_head;
				while (col_data) {
					row.Push(col_data->m_text);
					col_data = col_data->m_next;
				}
				while (row.Size() < (int)ep.m_lines.m_head->m_next->m_texts.m_size) {
					row.Push(std::string());
				}
				m_all_content.emplace_back(row);
				const string active = row[0];
				if (active.find('#') == string::npos) {
					m_content.insert(std::make_pair(atoi(row_data->m_texts.m_head->m_next->m_next->m_text), std::move(row)));
				}
				

			}
			++count;
			row_data = row_data->m_next;

		}
		
		return true;
	}

	void EditorConfigTab::Save(bool force /*= false*/)
	{
		Save(m_file.c_str(), force);
	}

	void EditorConfigTab::Save(const char* file, bool force /*= false*/)
	{
		if (!force && !m_modify) return;

		if (file == nullptr || file[0] == '\0') {
			printf(u8"EditorConfigTab::Save 表格文件名不能为空", file);
			return;
		}
		static char utf8[3]{ (char)0xEF, (char)0xBB, (char)0xBF};
		string path(file);
		path.append(file);
		ofstream os;
		os.open(file, ios::out|ios::binary);
		os.write(utf8, 3);
		for (int i = 0; i < CONFIG_HEAD_ROW_NUM; ++i){
			EditorConfigRow& row = m_head[i];
			for(int j = 0, k = row.Size(); j < k; ++j){
				const string& s = row[j];
				os << s.c_str();
				if (j < k - 1) {
					os << '\t';
				}
			}
			os << '\n';
		}

		for(auto it = m_all_content.begin(); it != m_all_content.end(); ++it){
			EditorConfigRow& row = *it;
			for (int j = 0, k = row.Size(); j < k; ++j) {
				const string& s = row[j];
				os << s.c_str();
				if (j < k - 1) {
					os << '\t';
				}
			}
			os << '\n';
		}
		os.flush();
		os.close();
		m_modify = false;
	}

	void EditorConfigTab::Reset()
	{
		Clear();
	}

	void EditorConfigTab::Clear()
	{
		m_head.clear();
		m_content.clear();
		m_all_content.clear();
	}

	void EditorConfigTab::ClearContent()
	{
		m_content.clear();
		m_all_content.clear();
	}

	void EditorConfigTab::Modify()
	{
		m_modify = true;
	}

	bool EditorConfigTab::IsModify() const
	{
		return m_modify;
	}

	EditorConfigRow EditorConfigTab::Find(int id)
	{
		const auto it = m_content.find(id);
		if (it != m_content.end()) {
			return it->second;
		}
		return EditorConfigRow();
	}

	void EditorConfigTab::Insert(const EditorConfigRow& row)
	{
		if (!row.Empty()) {
			m_content.insert(std::make_pair(row.GetID(), row));
			m_all_content.emplace_back(row);
			Modify();
		}
	}

	void EditorConfigTab::Replace(const EditorConfigRow& row)
	{
		if (!row.Empty()) {
			const auto it = m_content.find(row.GetID());
			if (it != m_content.end()) {
				it->second = row;
				for (int i = 0; i < (int)m_all_content.size(); ++i) {
					if ((m_all_content[i]).GetID() == row.GetID()) {
						m_all_content[i] = row;
						break;
					}
				}
			}else {
				m_content.insert(std::make_pair(row.GetID(), row));
				m_all_content.emplace_back(row);
			}
			Modify();
		}
	}

	void EditorConfigTab::Remove(int id)
	{
		{
			const auto it = m_content.find(id);
			if (it != m_content.end()) {
				m_content.erase(it);
			}
		}

		{
			for (auto it = m_all_content.begin(); it != m_all_content.end(); ++it) {
				if ((*it).GetID() == id) {
					m_all_content.erase(it);
					break;
				}
			}
		}
	}

//------------------------------------------------------------------------

	EditorConfigMark::EditorConfigMark()
		: m_file("")
		, m_magicid('M' | ('A' << 8) | ('R' << 16) | ('K' << 24))
		, m_version(1)
	{
	}

	EditorConfigMark::EditorConfigMark(const char* file)
		: m_file(file)
		, m_magicid('M' | ('A' << 8) | ('R' << 16) | ('K' << 24))
		, m_version(1)
	{
	}

	bool EditorConfigMark::Load()
	{
		return Load(m_file.c_str());
	}

	bool EditorConfigMark::Load(const char* file)
	{
		if (file == nullptr || file[0] == '\0') {
			printf(u8"EditorConfigMark::Load Mark文件名不能为空");
			return false;
		}

		/*IGxBlob* blob = sandbox::GetArchivePack()->GetBlob(file);
		if (!blob) {
			CERROR(u8"EditorConfigMark::Load Mark文件%s不存在", file);
			return false;
		}*/
		ifstream buf(file);
		string str((istreambuf_iterator<char>(buf)), istreambuf_iterator<char>());
		const char* buff = str.data();
		int seek = 0;
		//const char* buff = static_cast<char*>(blob->GetData());
		//const int size = blob->GetSize();
		// std::ifstream infile(pBuff.c_str());
		buf.seekg(0, ios_base::end);
		size_t fsize = buf.tellg();//list.json文本的大小
		buf.close();
		const int size = fsize;

		uint32_t magicid = *reinterpret_cast<const unsigned int*>(buff);
		seek += sizeof(unsigned int);
		uint32_t version = *reinterpret_cast<const unsigned int*>(buff + seek);
		seek += sizeof(unsigned int);

		if (m_magicid != m_magicid) {
			printf("EditorConfigMark::Load Mark文件%s Magic不匹配", file);
			return false;
		}

		/*if (m_version != version) {
			printf("EditorConfigMark::Load Mark文件%s Version不匹配", file);
			return false;
		}*/

		const unsigned int elementcount = *reinterpret_cast<const unsigned int*>(buff + seek);
		seek += sizeof(unsigned int);

		for (unsigned int i = 0; i < elementcount; i++)
		{
			Data data;
			data.m_posx = *reinterpret_cast<const float*>(buff + seek);
			seek += sizeof(float);
			data.m_posy = *reinterpret_cast<const float*>(buff + seek);
			seek += sizeof(float);
			data.m_posz = *reinterpret_cast<const float*>(buff + seek);
			seek += sizeof(float);
			data.m_refreshid = *reinterpret_cast<const unsigned int*>(buff + seek);
			seek += sizeof(unsigned int);

			const unsigned int markercount = *reinterpret_cast<const unsigned int*>(buff + seek);
			seek += sizeof(unsigned int);

			for (unsigned int j = 0; j < markercount; j++)
			{
				Mark mark;
				mark.m_snapmode = *reinterpret_cast<const int*>(buff + seek);
				seek += sizeof(int);
				mark.m_snapoffset = *reinterpret_cast<const float*>(buff + seek);
				seek += sizeof(float);
				mark.m_posx = *reinterpret_cast<const float*>(buff + seek);
				seek += sizeof(float);
				mark.m_posy = *reinterpret_cast<const float*>(buff + seek);
				seek += sizeof(float);
				mark.m_posz = *reinterpret_cast<const float*>(buff + seek);
				seek += sizeof(float);
				mark.m_dirw = *reinterpret_cast<const float*>(buff + seek);
				seek += sizeof(float);
				mark.m_dirx = *reinterpret_cast<const float*>(buff + seek);
				seek += sizeof(float);
				mark.m_diry = *reinterpret_cast<const float*>(buff + seek);
				seek += sizeof(float);
				mark.m_dirz = *reinterpret_cast<const float*>(buff + seek);
				seek += sizeof(float);
				data.m_marks.push_back(std::move(mark));
			}
			m_datas.push_back(std::move(data));
		}
		return true;
	}

	void EditorConfigMark::Save()
	{
		Save(m_file.c_str());
	}

	void EditorConfigMark::Save(const char* file)
	{
		if (file == nullptr || file[0] == '\0') {
			printf("EditorConfigMark::Save Mark文件名不能为空", file);
			return;
		}

		if (m_datas.empty()) {
			printf("EditorConfigMark::Save %s没有数据需要保存", file);
			return;
		}
	
		string path(ASSET_PATH);
		path.append(file);
		ofstream os;
		os.open(file, std::ios::binary | std::ios::out);
		os.write((char*)&m_magicid, sizeof(uint32_t));
		os.write((char*)&m_version, sizeof(uint32_t));
		uint32_t elementcount = (uint32_t)m_datas.size();
		os.write((char*)&(elementcount), sizeof(uint32_t));
		for (uint32_t i = 0; i < elementcount; ++i) {
			const Data& data = m_datas[i];
			os.write((char*)&data.m_posx, sizeof(float));
			os.write((char*)&data.m_posy, sizeof(float));
			os.write((char*)&data.m_posz, sizeof(float));
			os.write((char*)&data.m_refreshid, sizeof(uint32_t));
			uint32_t markcount = (uint32_t)data.m_marks.size();
			os.write((char*)&(markcount), sizeof(uint32_t));
			for (uint32_t j = 0; j < markcount; ++j) {
				const Mark& mark = data.m_marks[j];
				os.write((char*)&mark.m_snapmode, sizeof(int));
				os.write((char*)&mark.m_snapoffset, sizeof(float));
				os.write((char*)&mark.m_posx, sizeof(float));
				os.write((char*)&mark.m_posy, sizeof(float));
				os.write((char*)&mark.m_posz, sizeof(float));
				os.write((char*)&mark.m_dirw, sizeof(float));
				os.write((char*)&mark.m_dirx, sizeof(float));
				os.write((char*)&mark.m_diry, sizeof(float));
				os.write((char*)&mark.m_dirz, sizeof(float));
			}
		}
		os.flush();
		os.close();
	}
}
#ifdef CON_EDITOR

#endif