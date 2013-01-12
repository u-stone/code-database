#pragma once

//存放加载图片的信息
struct PicInfo 
{
	IPicture* _iPic;
	IStream* _iStream;
	HGLOBAL _hGlobal;
	int _width;
	int _height;
	OLE_XPOS_HIMETRIC _x;
	OLE_YPOS_HIMETRIC _y;
	OLE_XSIZE_HIMETRIC _cx;
	OLE_YSIZE_HIMETRIC _cy;

	PicInfo()
	{
		memset(this, 0, sizeof(PicInfo));
	}

	PicInfo& operator = (const PicInfo& obj)
	{
		if (this != &obj)
		{
			_iPic = obj._iPic;
			_iStream = obj._iStream;
			_hGlobal = obj._hGlobal;
			_width = obj._width;
			_height = obj._height;
			_x = obj._x;
			_y = obj._y;
			_cx = obj._cx;
			_cy = obj._cy;
		}
		return *this;
	}
};



class SwitchList2
{
public:
	SwitchList2(void);
	~SwitchList2(void);
private:
	PicInfo*m_DataList1[1];	//!<数据单元列表1
	PicInfo*m_DataList2[1];	//!<数据单元列表2

	//detail: 该值初始值为0，之后由写操作所在线程来修改其值，而读操作线程只能根据该值是多少进行相应的操作
	volatile long m_OpType;	//!<该值为0的时候表示当前List1为读，List2为写;该值为1的时候表示当前List1为写，List2为读

	size_t	m_ReadIndex;	//!<指示当前应该读取的数据单元的编号
	size_t	m_WriteIndex;	//!<指示当前可写的数据单元的编号
	bool	m_RequestWrite;	//!<指示当前处于读状态的列表是否请求进行写数据操作了
public:
	//将数据写入list中的下一个数据单元
	void putData(void* pDataUnit);
	//使用可读List中的下一个数据单元，并结合pParam来进行数据展示
	void useData(void* pParam);
};
