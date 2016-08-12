#pragma once
#include "KUtil.h"
#include "KConnection.h"
#include "KSocket.h"


class KServer:public KTransportBase
{
public:
	KServer()
	{
		ikcp_allocator(kMalloc,kFree);
	}

	~KServer()
	{
		m_kcpHeap.Clear();
		for (KHashMap<KConnection>::Iterator i=m_kcpHash.Begin();
			i!=m_kcpHash.End();++i)
		{
			delete (*i);
		}
		m_kcpHash.Clear();
	}

	int Bind(const sockaddr* addr){return m_socket.Bind(addr);}

	int Send(kcp_t kcp,const char* data,int len)
	{
		KConnection* conn=m_kcpHash.Find(kcp);
		int rslt=0;
		if (conn)
		{
			rslt=conn->Send(data,len);

			//����ǰ��connection�ŵ���ǰ��
			m_kcpHeap.Erase(conn);
			conn->m_checkTime=0;
			m_kcpHeap.Push(conn);
		}
		return rslt;
	}

	int Recv(kcp_t kcp,char* data,int len)
	{
		KConnection* conn=m_kcpHash.Find(kcp);
		int rslt=0;
		if (conn)
			rslt=conn->Recv(data,len);
		return rslt;
	}

	//����¼�
	int Wait(KEvent *ev,int evMax,int delay=-1);

protected:

	//����UDP��
	virtual int SendPacket(const struct sockaddr* addr,const char *buf, int len)
	{
		return m_socket.Sendto(buf,len,addr);
	}

	virtual void OutLog(const char *log)
	{
		printf("%s",log);
	}
private:
	KSocket            m_socket;
	KHeap<KConnection> m_kcpHeap;
	KHashMap<KConnection> m_kcpHash;
};
