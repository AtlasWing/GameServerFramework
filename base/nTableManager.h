#ifndef _N_TABLE_MANAGER_H_
#define _N_TABLE_MANAGER_H_

#include "base/Tidy.h"
#include "base/nRWLock.h"
#include "base/nSingleton.h"
#include "base/nManager.h"
#include "base/nObj.h"
#include "base/nStringTool.h"
#include "base/nXMLParser.h"
#include "base/nSTL.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	/// @brief 	表格文件*.tbl64的文件头信息结构
#pragma pack(1)
	struct TblHead
	{
		public:
			char    label[4];               ///< 始终为"tblx"
			DWORD   version;                ///< 版本号,tbl32为0x32000001,tbl64为0x64000001
			DWORD   encodingCode;           ///< 字符串编码页,目前为936 GB2312
			DWORD   segDataLength;          ///< 数据段长度
			DWORD   segStringPoolLength;    ///< 字符串池长度
			DWORD   segMetaInfoLength;      ///< 元信息段长度
		public:
			DWORD getDataOffset() const { return sizeof(TblHead); }
			DWORD getStringOffset() const { return sizeof(TblHead) + segDataLength; }
			DWORD getMetaOffset() const { return sizeof(TblHead) + segDataLength + segStringPoolLength; }
			DWORD memorySize() const { return segDataLength + segStringPoolLength ; }
	};
#pragma pack()

	/// @brief 表格字段类型及其所占的字节大小信息
	struct TableFieldTypeSize
	{
		const char* type;   ///< 字段类型
		size_t      size;   ///< 所占字节大小
	};


	/// @brief 表格字符串类型的前缀
	static std::string g_stringTypeName("string");

	template<typename ManagerT, bool needSingleton>
		class SingletonManager : public nSingleton<ManagerT>
	{
		public:
			SingletonManager()
			{
			}
			~SingletonManager()
			{
			}
	};
	template<typename ManagerT>
		class SingletonManager<ManagerT, false>
		{
			public:
				SingletonManager()
				{
				}
				~SingletonManager()
				{
				}
		};
	/// @brief	表格数据管理器
	/// @param	TableEntityT 数据在服务器内部格式类型
	/// @param	TableTblT 数据在TBL文件中格式类型
	template <class TableEntityT,class TableTblT, bool needSingleton = true>
		class  nTableManager : 
			public nManager<true, nIndexByID<CommonGetIDPolicy<TableEntityT>, Tidy::nHashTable<DWORD, TableEntityT*, CommonGetIDPolicy<TableEntityT> > >, nIndexByMultiName<CommonGetNamePolicy<TableEntityT> > >, 
			public SingletonManager<nTableManager<TableEntityT,TableTblT>, needSingleton>
	{
		protected:
			friend class nSingleton< nTableManager<TableEntityT,TableTblT> >;
			typedef nTableManager<TableEntityT, TableTblT, needSingleton> TableManagerType;
			typedef nManager<true, nIndexByID<CommonGetIDPolicy<TableEntityT>, Tidy::nHashTable<DWORD, TableEntityT*, CommonGetIDPolicy<TableEntityT> > >, nIndexByMultiName<CommonGetNamePolicy<TableEntityT> > > TableManagerBaseType;
			typedef nIndexByMultiName<CommonGetNamePolicy<TableEntityT> > IndexNameType;
			nTableManager() { }
			virtual ~nTableManager()
			{
			}

		public:
			/// @brief	表格大小
			size_t getSize(void)
			{
				return TableManagerBaseType::size();
			}
			/// @brief	遍历表格数据
			void listAll() const;
			/// @brief	用于遍历表格数据
			/// @param	exec 遍历回调函数
			/// @return	遍历操作是否成功
			template <typename ExecT>
				bool execEveryTable(ExecT &exec)
				{
					return execAll(exec);
				}
			template <typename ExecT>
				bool execEveryTable(ExecT &exec)const
				{
					return execAll(exec);
				}
			/// @brief	根据ID获得数据
			/// @param	id 指定ID	
			/// @return	返回数据指针，未找到返回NULL
			const TableEntityT* getTableByID(const DWORD id) const
			{
				return TableManagerBaseType::getByID(id);
			}
			const TableEntityT* getTableByID(const DWORD id, bool needLock) const
			{
				if(needLock == true){
					const_cast<TableManagerType*>(this)->rdlock();
				}
				const TableEntityT *result = TableManagerBaseType::getByID(id);
				if(needLock == true){
					const_cast<TableManagerType*>(this)->unlock();
				}
				return result;
			}
			bool checkHasTableByID(const DWORD id, bool needLock)
			{
				if(needLock == true){
					rdlock();
				}
				const bool result = (TableManagerBaseType::getByID(id) != NULL);
				if(needLock == true){
					unlock();
				}
				return result;
			}
			/// @brief	根据Name获得数据
			/// @param	name 指定名称
			/// @param	random 当有多个数据时是否随机取一个
			/// @return	返回数据指针，未找到返回NULL
			const TableEntityT* getTableByName( const char* name, bool randomSelect = false )
			{
				return TableManagerBaseType::getByName(name, randomSelect);;
			}

		public:
			bool load(std::string tblFileName, const bool isFirstLoad, const bool needLock) {
				return freshData(tblFileName, isFirstLoad, needLock);
			}
			bool fresh(std::string tblFileName, const bool needLock) {
				return freshData(tblFileName, false, needLock);
			}
			void wrlock(){
				_data_lock.wrlock();
			}
			void rdlock(){
				_data_lock.rdlock();
			}
			void unlock(){
				_data_lock.unlock();
			}

		private:
			/// @brief	加载表格
			/// @param	tblFileName 文件名
			// 表格加载过程：
			// 1。从文件中读取文件头stTblFileHead
			// 2。根据文件头的信息集中分配数据段和字符串段所需要的总内存
			// 3。根据文件头信息读取数据段与字符串段(如果有)
			// 4。如果没有字符串段则读取完成，跳转6
			// 5。遍历数据，并将数据段的字符串指针指向字符串段中的正确地址。
			// 6。读取完成
			bool freshData(std::string tblFileName, const bool isFirstLoad, const bool needLock);
			/// @brief	单条记录从TableTblT到TableEntityT类型的转换
			/// @param	tblData 要转换的一条记录	
			/// @return	转换是否成功	
			bool refreshData(TableTblT &tblData, const bool isFirstLoad);
			/// @brief 由output元信息得到字段信息
			/// @return 每条记录的大小，返回0表示解析错误
			bool GetFieldInfo(const TiXmlElement* output, std::vector<int>& relocationTemplateOUT, size_t& itemSizeOUT);

		private:
			virtual void dealStartLoad(){
			}
			virtual void dealCompleteLoad(){
			}

		private:
			nRWLock _data_lock;
	};

	namespace{
		template<typename TableEntityT>
			struct ListAllExec {
				ListAllExec() {}
				bool exec(TableEntityT *entity)
				{
					entity->debug("加载的表格数据(id,名称):(%u, %s)", entity->id, entity->name);
					return true;
				}
			};
	};
	template <class TableEntityT,class TableTblT, bool needSingleton>
		void nTableManager<TableEntityT, TableTblT, needSingleton>::listAll() const
		{
			ListAllExec<TableEntityT> exec;
			execEveryTable(exec);
		}

	template <class TableEntityT,class TableTblT, bool needSingleton>
		bool nTableManager<TableEntityT, TableTblT, needSingleton>::freshData(std::string tblFileName, const bool isFirstLoad, const bool needLock)
		{
			std::string filename = (const char*)Tidy::global["tableDir"];
			filename += tblFileName;
			FILE* fp = fopen(filename.c_str(),"rb");
			if(fp == NULL) {
				baseServer->error("打开文件失败 %s",filename.c_str());
				assert_fail("打开文件失败");
				return false;
			}
			bool ret = true;
			do{
				// 文件头读取
				TblHead head;
				fread( &head, sizeof(head),1,fp );
				if( memcmp(head.label, "tblx", 4) != 0 
						|| head.version != 1
						|| head.segMetaInfoLength == 0){
					assert_fail("失败");
					ret = false;
					break;
				}

				// 元数据读取
				std::string metaInfo;
				metaInfo.resize(head.segMetaInfoLength);
				fseek(fp, head.getMetaOffset(), SEEK_SET);
				fread(const_cast<char*>(metaInfo.c_str()),metaInfo.size(),1,fp);

				// 元数据解析
				TiXmlDocument output;
				output.Parse(reinterpret_cast<const char*>(metaInfo.c_str()), 0, TIXML_ENCODING_UTF8);
				if(output.Error()){
					assert_fail("");
					ret = false;
					break;
				}

				// 由元数据解析字段信息
				std::vector<int> relocationTemplateOUT;
				size_t itemSize = 0;
				if(GetFieldInfo(output.RootElement(), relocationTemplateOUT, itemSize) == false
						|| itemSize == 0 
						|| head.segDataLength % itemSize != 0){
					assert_fail("数据错误");
					ret = false;
					break;
				}

				assert_debug(itemSize == sizeof(TableTblT), "哼哼,那个小伙改了表格没提交?");

				// 数据区读取
				BYTE* pTableData = new BYTE[head.segDataLength + head.segStringPoolLength];
				fseek(fp, head.getDataOffset(), SEEK_SET);
				fread( pTableData, head.segDataLength + head.segStringPoolLength, 1, fp );
				const size_t itemCount = head.segDataLength / itemSize;

				// 有必要的情况下进行字符串重定位
				if(relocationTemplateOUT.empty() == false) {
					//assert(*(pTableData + head.segDataLength) == 0 && "字符串池起始0哨岗检测" ); 	
					// 字符串池段的首地址
					const QWORD stringPoolAddress = reinterpret_cast<QWORD>(pTableData + head.segDataLength);

					// 变长字符串指针重定位
					for(DWORD i = 0; i < itemCount; i++)
					{
						BYTE* item = pTableData + i * itemSize;
						for(std::vector<int>::const_iterator it = relocationTemplateOUT.begin(); it != relocationTemplateOUT.end(); ++it)
						{
							QWORD* p = reinterpret_cast<QWORD*>(item + *it);
							*p += stringPoolAddress;
							assert_debug( (*p == (QWORD)stringPoolAddress) || (*(((BYTE*)*p) - 1) == 0), "字符串池0休止符检测");
							assert_debug(*p >= stringPoolAddress && *p <= stringPoolAddress + head.segStringPoolLength, "确保字符串映射在字符串池范围内" );
						}
					}
					assert_debug(*(pTableData + head.memorySize() - 1) == 0, "字符串池最后的0休止符检测");
				}

				if(needLock == true){
					wrlock();
				}
				dealStartLoad();
				/// 序列化数据内容,从TableTblT到TableEntityT的转换
				TableTblT *tblData = (TableTblT*)pTableData;
				for(DWORD i =0; i<itemCount; ++i) {
					if(refreshData(*tblData, isFirstLoad) == false) {
						ret = false;
						assert_fail("刷新数据错误");
						break;
					}
					tblData++;
				}
				dealCompleteLoad();
				SAFE_DELETE(pTableData);		
				if(needLock == true){
					unlock();
				}
			}while(0);

			fclose(fp);
			if(ret == false) {
				baseServer->error("刷新基本表 %s 失败",filename.c_str());
				assert_fail("刷新基本表失败");
			}

			return ret;
		}

	template <class TableEntityT, class TableTblT, bool needSingleton>
		bool nTableManager<TableEntityT, TableTblT, needSingleton>::refreshData(TableTblT &tblData, const bool isFirstLoad)
		{
			TableEntityT* data = (TableEntityT*)getTableByID( tblData.getUniqueID() );
			/// 如果管理器中没有，则添加
			if(data == NULL) {
				data = new TableEntityT();
				data->reset();
				if(data->fill(tblData) == false){
					assert_fail("表格加载失败");
					SAFE_DELETE(data);
					return false;
				}
				if(!add(data)) {
					SAFE_DELETE(data);
					return false;
				}
			}
			/// 如果管理器中已经有，则刷新数据
			else {
				assert_debug(isFirstLoad == false, "数据重复");
				IndexNameType::remove(data);
				data->reset();
				// 通过析构和构造自动清空vector,map等容器
				data->~TableEntityT();
				Tidy::constructInPlace(data);
				data->reset();
				if(data->fill(tblData) == false){
					assert_fail("表格加载失败");
					return false;
				}
				IndexNameType::add(data);
			}
			return true;
		}

	template <class TableEntityT, class TableTblT, bool needSingleton>
		bool nTableManager<TableEntityT, TableTblT, needSingleton>::GetFieldInfo(const TiXmlElement* output, std::vector<int>& relocationTemplateOUT, size_t& itemSizeOUT)
		{
			/// @brief 全局的字段类型和字节大小信息表
			static TableFieldTypeSize g_tableFieldTypeSize[] = 
			{
				{"uint8",       1},
				{"int8",        1},
				{"uint16",      2},
				{"int16",       2},
				{"uint32",      4},
				{"int32",       4},
				{"uint64",      8},
				{"int64",       8},
				{"float32",     4},
				{"float64",     8},
			};
#define count_of(X) (sizeof(X)/sizeof((X)[0]))
			if(output != NULL
					&& nStringTool::compare(output->ValueStr(), "output") == 0
					&& nStringTool::compare(output->Attribute("type"), "tbl64") == 0)
			{
				// 结果集初始化
				relocationTemplateOUT.clear();
				itemSizeOUT = 0;

				// 遍历字段进行元数据解析
				for(const TiXmlElement* field = output->FirstChildElement("field"); field != NULL; field = field->NextSiblingElement("field"))
				{
					const char* type = field->Attribute("type");
					if(type == NULL)
						return false;

					// 数值类型解析
					bool find = false;
					for(size_t i = 0; i < count_of(g_tableFieldTypeSize); i++)
					{
						if(nStringTool::compare(type, g_tableFieldTypeSize[i].type) == 0)
						{
							itemSizeOUT += g_tableFieldTypeSize[i].size;
							find = true;
							break;
						}
					}

					if(find == false && strncmp(type, g_stringTypeName.c_str(), g_stringTypeName.size()) == 0)
					{
						// 变长字符串解析
						if(type[g_stringTypeName.size()] == '\0')
						{
							relocationTemplateOUT.push_back(itemSizeOUT);
							itemSizeOUT += sizeof(void*);
						}
						// 定长字符串解析
						else
						{
							const int size = atoi(type + g_stringTypeName.size());
							if(size == 0 || size == INT_MAX || size == INT_MIN)
								return false;
							itemSizeOUT += size;
						}
					}
				}
				return true;
			}
			return false;
		}
};
#endif
