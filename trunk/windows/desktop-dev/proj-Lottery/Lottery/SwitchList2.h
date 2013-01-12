#pragma once

//��ż���ͼƬ����Ϣ
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
	PicInfo*m_DataList1[1];	//!<���ݵ�Ԫ�б�1
	PicInfo*m_DataList2[1];	//!<���ݵ�Ԫ�б�2

	//detail: ��ֵ��ʼֵΪ0��֮����д���������߳����޸���ֵ�����������߳�ֻ�ܸ��ݸ�ֵ�Ƕ��ٽ�����Ӧ�Ĳ���
	volatile long m_OpType;	//!<��ֵΪ0��ʱ���ʾ��ǰList1Ϊ����List2Ϊд;��ֵΪ1��ʱ���ʾ��ǰList1Ϊд��List2Ϊ��

	size_t	m_ReadIndex;	//!<ָʾ��ǰӦ�ö�ȡ�����ݵ�Ԫ�ı��
	size_t	m_WriteIndex;	//!<ָʾ��ǰ��д�����ݵ�Ԫ�ı��
	bool	m_RequestWrite;	//!<ָʾ��ǰ���ڶ�״̬���б��Ƿ��������д���ݲ�����
public:
	//������д��list�е���һ�����ݵ�Ԫ
	void putData(void* pDataUnit);
	//ʹ�ÿɶ�List�е���һ�����ݵ�Ԫ�������pParam����������չʾ
	void useData(void* pParam);
};
