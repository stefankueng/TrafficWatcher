#ifndef __PACKET_H__
#define __PACKET_H__


#include	"stdafx.h"
#include	"thread.h"
#include	"ntddndis.h"
#include	<winsock2.h>
#include	"iphlpapi.h"
#include	"ipstructs.h"

#define MAX_ADAPTERS				100
#define	MAX_ADAPTERNAME_LENGTH		512

#ifdef __cplusplus
extern "C" {
#endif

BOOLEAN PacketSetMinToCopy(LPADAPTER AdapterObject,int nbytes);
BOOLEAN PacketSetNumWrites(LPADAPTER AdapterObject,int nwrites);
BOOLEAN PacketSetMode(LPADAPTER AdapterObject,int mode);
BOOLEAN PacketSetMaxLookaheadsize (LPADAPTER AdapterObject);
BOOLEAN PacketSetReadTimeout(LPADAPTER AdapterObject,int timeout);
BOOLEAN PacketSetBpf(LPADAPTER AdapterObject,struct bpf_program *fp);
BOOLEAN PacketGetStats(LPADAPTER AdapterObject,struct bpf_stat *s);
BOOLEAN PacketSetBuff(LPADAPTER AdapterObject,int dim);
BOOLEAN PacketGetNetType (LPADAPTER AdapterObject,NetType *type);
LPADAPTER PacketOpenAdapter(LPTSTR AdapterName);
BOOLEAN PacketSendPacket(LPADAPTER AdapterObject,LPPACKET pPacket,BOOLEAN Sync);
LPPACKET PacketAllocatePacket(void);
LPPACKET PacketAllocateNPacket(UINT n);
VOID PacketInitPacket(LPPACKET lpPacket,PVOID  Buffer,UINT  Length);
VOID PacketFreePacket(LPPACKET lpPacket);
BOOLEAN PacketResetAdapter(LPADAPTER AdapterObject);
BOOLEAN PacketWaitPacket(LPADAPTER AdapterObject,LPPACKET lpPacket);
BOOLEAN PacketReceiveNPacket(LPADAPTER AdapterObject,LPPACKET headLPacket,UINT n,UINT length,BYTE* buffer,BOOLEAN Sync);
BOOLEAN PacketReceivePacket(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync);
VOID PacketCloseAdapter(LPADAPTER lpAdapter);
BOOLEAN PacketSetHwFilter(LPADAPTER AdapterObject,ULONG Filter);
BOOLEAN PacketGetAdapterNames(PTSTR pStr,PULONG  BufferSize);
BOOLEAN PacketGetNetInfo(LPTSTR AdapterName, PULONG netp, PULONG maskp);
BOOLEAN PacketRequest(LPADAPTER  AdapterObject,BOOLEAN Set,PPACKET_OID_DATA  OidData);
VOID PacketSetNextPacket(LPPACKET lpPacket, LPPACKET next);
VOID PacketSetLengthBuffer(LPPACKET lpPacket, UINT dim);
VOID PacketSetLengthPacket(LPPACKET lpPacket, UINT numBytes);
LPPACKET PacketGetNextPacket(LPPACKET lpPacket);
#ifdef __cplusplus
}
#endif 

/*!
 * class which holds all necessary information and data of a network adapter.
 */
class CNetAdapter
{
public:
	LPADAPTER	pAdapter;				///< pointer to an ADAPTER struct. needed for packet.dll
	UINT		Type;					///< holds the type of the adapter
	CString		AdapterString;			///< the name of the adapter as found in the registry
	CString		AdapterDescription;		///< the description of the adapter (readable name)
	ULONG		ip;						///< the ip address of the adapter
	CString		ip_str;					///< the ip address of the adapter as a string (format: ddd.ddd.ddd.ddd)
	ULONG		mask;					///< the netmask of the adapter
	CString		mask_str;				///< the netmask of the adapter as a string (format: ddd.ddd.ddd.ddd)
	CString		gateway_str;			///< the ip of the main gateway
	LPPACKET	pPacket;				///< pointer to PACKET structure which hold the captured data
	CHAR		*pBuffer;				///< pointer to the captured data
	UINT		AddressLength;			///< length of the hardware address
	CString		Address;				///< specifies the hardware address for the adapter

};



/**
 * CPacket.
 * a wrapper class for the packet capture library. 
 * see <A HREF="http://netgroup-serv.polito.it/winpcap/">http://netgroup-serv.polito.it/winpcap/</A>.
 * to compile this class, you must download the packet.lib file from the website and link it.
 * to make the created program work, you must also install the winpcap driver and packet.dll. an
 * installer for these is also available on the website.
 *
 * this class is not complete. not all functions of the packet.dll are used and implemented.
 * I just tried to write a class which is easy to use and does many things automatically.
 *
 * usage:
 * inherit your class from CPacket. then implement the method AnalyzePackets(). This method will be
 * called for every packet received.
 * see detailed documentation of each method.
 * @code
 * class CAnalyze : CPacket
 * {
 *		void start();
 *		void AnalyzePackets(UCHAR *buffer, UCHAR *packet);
 *		void stop();
 * }
 *
 * CAnalyze::start()
 * {
 *		init();				//call init() from the base class
 *		Open();				//call Open() from the base class
 *		//now the thread is running and AnalyzePackets is called for every packet
 * }
 *		
 * CAnalyze::AnalyzePackets(UCHAR *buffer, UCHAR *packet)
 * {
 *		//now analyse each packet here...
 * }
 * CAnalyze::stop()
 * {
 *		Stop();				//call Close() from the base class
 * }
 * @endcode
 * now you can instantiate CAnalyze in another class
 * @code
 * CAnalyze	m_analyze;
 * ...
 * m_analyze.start();
 * ...
 * m_analyze.stop();
 * ...
 * @endcode
 *
 */

class	CPacket : public CThread , CNetAdapter
{
public:
	CPacket() {nActiveAdapter = -1;};
	~CPacket();
	

    /*!
     * initializes the CPacket object. you must call this class before
	 * a call to any other method of this class. retrieves information
	 * about all available network adapters on the local computer.
     *
     * @return  :	TRUE if working adapters found, FALSE otherwise
     */
	BOOL			init();
	int				GetNumberOfAdapters() {return nAdapterCount;};
	int				GetActiveAdapterNumber() {return nActiveAdapter;};
	CString			GetIP() {if (nActiveAdapter < 0) {return "127.0.0.1";} else return Adapters[nActiveAdapter].ip_str;};
	CString			GetIP(int adapter) {return Adapters[adapter].ip_str;};
	CString			GetMask() {if (nActiveAdapter < 0) {return "255.255.255.255";} else return Adapters[nActiveAdapter].mask_str;};
	CString			GetGateway() {if (nActiveAdapter < 0) {return "127.0.0.1";} else return Adapters[nActiveAdapter].gateway_str;};
	CString			GetDescription() {if (nActiveAdapter < 0) {return "nothing";} else return Adapters[nActiveAdapter].AdapterDescription;};
	CString			GetDescription(int adapter) {return Adapters[adapter].AdapterDescription;};
	CString			GetMacAddr() {if (nActiveAdapter < 0) {return "none";} else return Adapters[nActiveAdapter].Address;};

    /*!
     * Opens an adapter, sets the size of the application and kernel buffer and starts the sniffing thread.
     *
     * @param i : the number of the adapter to open. if i is -1, then a valid adapter is determined automatically.
     * @param bufsize : the size of the application buffer to use. smaller buffer lead to increased CPU usage.
     * @param kernelbuf : the size of the kernel mode buffer used by the capture driver itself. smaller buffer lead to increased CPU usage.
     * @param promiscuous : set to FALSE if adapter should not work in promiscuous mode
	 *
     * @return  : TRUE if the adapter could be opened and thread could be started.
     */
	BOOL			Open(int i = (-1), DWORD bufsize = 524288, DWORD kernelbuf = 524288, BOOL promiscuous = TRUE);

    /*!
     * Closes all adapters and frees all allocated memory
     *
     * @return  : TRUE if successful
     */
	BOOL			Close();

protected://members
	
    //! number of network adapters found. WARNING: also loopback devices are listed
	int				nAdapterCount;
	
    //! specifies the current adapter which is in use by this class
	int				nActiveAdapter;

	
    //! array to hold the information about ALL found adapters
	CNetAdapter		Adapters[MAX_ADAPTERS];

protected://methods
	/**
	 * override this method.
	 * this method is called whenever a packet is received. it is called once
	 * for every packet, even if more than one packets were received at once.
	 * @param *buffer		pointer to a PACKET structure
	 * @param *packet		pointer to the start of the packet data
	 */
	void virtual	AnalyzePackets(UCHAR *buffer, UCHAR *packet) = 0;


    /*!
     * Opens the specified adapter for further use
     *
     * @param i : the number of the adapter (zero based)
     *
     * @return  : a pointer to an ADAPTER structure assigned to this adapter
     */
	LPADAPTER		OpenAdapter(int i);

    /*!
     * closes the specified adapter and releases the assigned ADAPTER structure
     *
     * @param i : the number of the adapter (zero based)
     */
	void			CloseAdapter(int i);

    /*!
     * checks if a given adapter is valid. that means if the adapter has a 'good' ip address other than 0.0.0.0 
	 * and also not the loopback ip of 127.0.0.1
     *
     * @param i : the number of the adapter (zero based)
     *
     * @return  : TRUE if the adapter is valid, FALSE otherwise
     */
	BOOL			IsValidIPAdapter(int i);


private://methods

	/*!
	 * reads the packets from the driver. calls AnalyzePackets() for every
	 * packet received.
	 */
	DWORD			ThreadMethod();

	/*! retrieves all adapters from the system registry */
	//BOOL			GetAdapterNames();

    /*!
     * reads out some information about the adapter like ip address, netmask, ...
     * and fills this information into the NetAdapter structure
     * @param i : the number of the adapter (zero based)
     *
     * @return  : TRUE if methods succeeds
     */
	//BOOL			GetNetInfo(int i);


private://members
    //! marks the begin of a received packet
	UCHAR			*packetbegin;
    //! marks the end of a received packet
	UCHAR			*packetend;


};


#endif