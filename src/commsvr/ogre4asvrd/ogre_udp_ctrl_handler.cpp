#include "ogre_predefine.h"
#include "ogre_buf_storage.h"
#include "ogre_configure.h"
#include "ogre_ip_restrict.h"
#include "ogre_udp_ctrl_handler.h"

//����UPD�˿ڵľ��
std::vector<Ogre_UDPSvc_Hdl *> Ogre_UDPSvc_Hdl::ary_upd_peer_;

//���캯��
Ogre_UDPSvc_Hdl::Ogre_UDPSvc_Hdl(const ZCE_Sockaddr_In &upd_addr, ZCE_Reactor *reactor):
    ZCE_Event_Handler(reactor),
    udp_bind_addr_(upd_addr),
    peer_svc_info_(upd_addr.get_ip_address(), upd_addr.get_port_number()),
    dgram_databuf_(NULL),
    ip_restrict_(Ogre_IPRestrict_Mgr::instance())
{
    //���浽PEER����
    ary_upd_peer_.push_back(this);
}

//��������
Ogre_UDPSvc_Hdl::~Ogre_UDPSvc_Hdl()
{
    if (dgram_databuf_ != NULL)
    {
        Ogre_Buffer_Storage::instance()->free_byte_buffer(dgram_databuf_);
    }

    //ɾ���������PEER����
    std::vector<Ogre_UDPSvc_Hdl *>::iterator iter_tmp = ary_upd_peer_.begin();
    std::vector<Ogre_UDPSvc_Hdl *>::iterator iter_end = ary_upd_peer_.begin();

    for (; iter_tmp != iter_end; ++iter_tmp)
    {
        if (*iter_tmp == this)
        {
            ary_upd_peer_.erase(iter_tmp);
            break;
        }
    }
}

//��ʼ��һ��UDP PEER
int Ogre_UDPSvc_Hdl::init_udp_peer()
{

    dgram_databuf_ = Ogre_Buffer_Storage::instance()->allocate_byte_buffer();

    int ret = 0;
    ret = dgram_peer_.open(&udp_bind_addr_);
    if (ret != 0)
    {
        handle_close();
        return -1;
    }

    ret = reactor()->register_handler (this, ZCE_Event_Handler::READ_MASK );

    if (ret != 0)
    {
        return -2;
    }

    return 0;
}

//ȡ�þ��
ZCE_HANDLE Ogre_UDPSvc_Hdl::get_handle(void) const
{
    return (ZCE_HANDLE)dgram_peer_.get_handle();
}

int Ogre_UDPSvc_Hdl::handle_input(ZCE_HANDLE)
{
    size_t szrevc = 0;
    int ret = 0;
    ZCE_Sockaddr_In remote_addr;
    //��ȡ����
    ret = read_data_fromudp(szrevc, remote_addr);

    ZCE_LOGMSG_DBG(RS_DEBUG, "UDP Read Event[%s|%u].UPD Handle input event triggered. ret:%d,szrecv:%u.\n",
                   remote_addr.get_host_addr(),
                   remote_addr.get_port_number(),
                   ret,
                   szrevc);

    if (ret != 0)
    {
        //return -1�𣬵������治֪����δ���
        //return -1;
    }

    //��������ɹ�
    if (szrevc > 0)
    {
        pushdata_to_recvpipe();
    }

    return 0;
}

//
int Ogre_UDPSvc_Hdl::handle_close ()
{
    //
    if (dgram_peer_.get_handle () != ZCE_INVALID_SOCKET)
    {
        reactor()->remove_handler (this, false);
        dgram_peer_.close ();
    }

    //ɾ���Լ�
    delete this;

    return 0;
}

//��ȡUDP����
int Ogre_UDPSvc_Hdl::read_data_fromudp(size_t &szrevc, ZCE_Sockaddr_In &remote_addr)
{
    int ret = 0;
    szrevc = 0;
    ssize_t recvret = 0;

    recvret = dgram_peer_.recvfrom(dgram_databuf_->frame_data_,
                                   Ogre4a_App_Frame::MAX_OF_OGRE_DATA_LEN,
                                   0,
                                   &remote_addr);

    if (recvret < 0)
    {

        if (ZCE_LIB::last_error() != EWOULDBLOCK )
        {
            //�����ж�,�ȴ�����
            if (ZCE_LIB::last_error() == EINVAL)
            {
                return 0;
            }

            //��¼����,���ش���
            ZLOG_ERROR( "UDP Read error [%s|%u],receive data error peer:%u ZCE_LIB::last_error()=%d|%s.\n",
                        remote_addr.get_host_addr(),
                        remote_addr.get_port_number(),
                        dgram_peer_.get_handle(),
                        ZCE_LIB::last_error(),
                        strerror(ZCE_LIB::last_error()));
            return SOAR_RET::ERR_OGRE_SOCKET_OP_ERROR;
        }
        else
        {
            return 0;
        }
    }

    //������������ӵķ�������ַ�м�û��.�����ھܾ��ķ����б���... kill
    ret =  ip_restrict_->check_ip_restrict(remote_addr) ;

    if (ret != 0)
    {
        return ret;
    }

    //Socket���رգ�Ҳ���ش����ʾ,�����Ҳ�֪���᲻������������...
    if (recvret == 0)
    {
        ZLOG_ERROR( "UDP Read error [%s|%u].UDP Peer recv return 0, I don't know how to process.?\n",
                    remote_addr.get_host_addr(),
                    remote_addr.get_port_number());
        return SOAR_RET::ERR_OGRE_SOCKET_CLOSE;
    }

    dgram_databuf_->snd_peer_info_.set(remote_addr);
    dgram_databuf_->rcv_peer_info_ = this->peer_svc_info_;
    dgram_databuf_->ogre_frame_len_ = Ogre4a_App_Frame::LEN_OF_OGRE_FRAME_HEAD + recvret;
    //���ⷢ����������д�����
    dgram_databuf_->ogre_frame_option_ = 0;
    dgram_databuf_->ogre_frame_option_ |= Ogre4a_App_Frame::OGREDESC_PEER_UDP;

    szrevc = recvret;

    return 0;
}

int Ogre_UDPSvc_Hdl::pushdata_to_recvpipe()
{

    int ret = Soar_MMAP_BusPipe::instance()->push_back_bus(Soar_MMAP_BusPipe::RECV_PIPE_ID,
                                                           reinterpret_cast<ZCE_LIB::dequechunk_node *>(dgram_databuf_));

    //���۴�����ȷ���,���ͷŻ������Ŀռ�

    //��־�ں����������,�����.
    if (ret != 0 )
    {
        return SOAR_RET::ERR_OGRE_RECEIVE_PIPE_IS_FULL;
    }

    return 0;
}


//����UDP���ݡ�
int Ogre_UDPSvc_Hdl::send_alldata_to_udp(Ogre4a_App_Frame *send_frame)
{
    ssize_t szsend = -1;

    const size_t BUFFER_LEN = 32;
    char buffer[BUFFER_LEN + 1];

    ZCE_Sockaddr_In remote_addr(send_frame->rcv_peer_info_.peer_ip_address_,
                                send_frame->rcv_peer_info_.peer_port_);

    //
    size_t i = 0;

    for (; i < ary_upd_peer_.size(); ++i)
    {
        //�ҵ���Ӧ�Ķ˿ڣ��ж��UDP�Ķ˿ڣ�ѡ��ҵ����ע�Ķ˿ڷ���
        if (ary_upd_peer_[i]->peer_svc_info_ == send_frame->snd_peer_info_ )
        {
            szsend = ary_upd_peer_[i]->dgram_peer_.sendto(send_frame->frame_data_,
                                                          send_frame->ogre_frame_len_ - Ogre4a_App_Frame::LEN_OF_OGRE_FRAME_HEAD,
                                                          0,
                                                          &remote_addr);
            break;
        }
    }

    //û���ҵ���Ӧ�Ķ˿ڣ�����һ����Ϣ
    if (i == ary_upd_peer_.size())
    {
        ZLOG_ERROR( "Can't find send peer[%s|%u].Please check code.\n",
                    ZCE_LIB::inet_ntoa(send_frame->snd_peer_info_.peer_ip_address_, buffer, BUFFER_LEN),
                    send_frame->snd_peer_info_.peer_port_);
        return SOAR_RET::ERR_OGRE_SOCKET_OP_ERROR;
    }

    //����ʧ��
    if (szsend <= 0)
    {
        ZLOG_ERROR( "UDP send error[%s|%u]. Send data error peer:%u ZCE_LIB::last_error()=%d|%s.\n",
                    remote_addr.get_host_addr(),
                    remote_addr.get_port_number(),
                    ary_upd_peer_[i]->get_handle(),
                    ZCE_LIB::last_error(),
                    strerror(ZCE_LIB::last_error()));
        return SOAR_RET::ERR_OGRE_SOCKET_OP_ERROR;
    }

    ZCE_LOGMSG_DBG(RS_DEBUG, "UDP Send data to peer [%s|%u]  Socket %u bytes data Succ.\n",
                   remote_addr.get_host_addr(),
                   remote_addr.get_port_number(),
                   szsend);
    return 0;
}