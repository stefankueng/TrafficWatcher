#ifndef __IPSTRUCTS_H__
#define __IPSTRUCTS_H__



#if _MSC_VER > 1000
#pragma once

#ifndef ANY_SIZE
#define ANY_SIZE 1
#endif

//
#define DEVICE_TYPE ULONG
#define FILE_DEVICE_BEEP                0x00000001
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_CONTROLLER          0x00000004
#define FILE_DEVICE_DATALINK            0x00000005
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_INPORT_PORT         0x0000000a
#define FILE_DEVICE_KEYBOARD            0x0000000b
#define FILE_DEVICE_MAILSLOT            0x0000000c
#define FILE_DEVICE_MIDI_IN             0x0000000d
#define FILE_DEVICE_MIDI_OUT            0x0000000e
#define FILE_DEVICE_MOUSE               0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SCANNER             0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_SCREEN              0x0000001c
#define FILE_DEVICE_SOUND               0x0000001d
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_TRANSPORT           0x00000021
#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_DEVICE_VIDEO               0x00000023
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_WAVE_IN             0x00000025
#define FILE_DEVICE_WAVE_OUT            0x00000026
#define FILE_DEVICE_8042_PORT           0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028
#define FILE_DEVICE_BATTERY             0x00000029
#define FILE_DEVICE_BUS_EXTENDER        0x0000002a
#define FILE_DEVICE_MODEM               0x0000002b
#define FILE_DEVICE_VDM                 0x0000002c
#define FILE_DEVICE_MASS_STORAGE        0x0000002d

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

#define FILE_ANY_ACCESS                 0
#define FILE_READ_ACCESS          ( 0x0001 )	// file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )	// file & pipe


//defines and structs taken from ntddpack.h
struct _PACKET_OID_DATA {
    ULONG Oid;
    ULONG Length;
    UCHAR Data[1];
}; 

typedef struct _PACKET_OID_DATA PACKET_OID_DATA, *PPACKET_OID_DATA;

/*#include <packoff.h> */
#define FILE_DEVICE_PROTOCOL        0x8000
#define IOCTL_PROTOCOL_QUERY_OID    CTL_CODE(FILE_DEVICE_PROTOCOL, 0 , METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_SET_OID      CTL_CODE(FILE_DEVICE_PROTOCOL, 1 , METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_STATISTICS   CTL_CODE(FILE_DEVICE_PROTOCOL, 2 , METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_RESET        CTL_CODE(FILE_DEVICE_PROTOCOL, 3 , METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_READ         CTL_CODE(FILE_DEVICE_PROTOCOL, 4 , METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_WRITE        CTL_CODE(FILE_DEVICE_PROTOCOL, 5 , METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTOCOL_MACNAME      CTL_CODE(FILE_DEVICE_PROTOCOL, 6 , METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OPEN                  CTL_CODE(FILE_DEVICE_PROTOCOL, 7 , METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CLOSE                 CTL_CODE(FILE_DEVICE_PROTOCOL, 8 , METHOD_BUFFERED, FILE_ANY_ACCESS)

//defines and structs taken from packet32.h
//working modes
#define MODE_CAPT 0
#define MODE_STAT 1

//ioctls
#define	 pBIOCSETBUFFERSIZE 9592		///< IOCTL code: set kernel buffer size.
#define	 pBIOCSETF 9030					///< IOCTL code: set packet filtering program.
#define  pBIOCGSTATS 9031				///< IOCTL code: get the capture stats.
#define	 pBIOCSRTIMEOUT 7416			///< IOCTL code: set the read timeout.
#define	 pBIOCSMODE 7412				///< IOCTL code: set working mode.
#define	 pBIOCSWRITEREP 7413			///< IOCTL code: set number of physical repetions of every packet written by the app.
#define	 pBIOCSMINTOCOPY 7414			///< IOCTL code: set minimum amount of data in the kernel buffer that unlocks a read call.
#define	 pBIOCSETOID 2147483648			///< IOCTL code: set an OID value.
#define	 pBIOCQUERYOID 2147483652		///< IOCTL code: get an OID value.
#define	 pATTACHPROCESS 7117			///< IOCTL code: attach a process to the driver. Used in Win9x only.
#define	 pDETACHPROCESS 7118			///< IOCTL code: detach a process from the driver. Used in Win9x only.
#define  pBIOCSETDUMPFILENAME 9029		///< IOCTL code: set the name of a the file used by kernel dump mode.
#define  pBIOCEVNAME 7415				///< IOCTL code: get the name of the event that the driver signals when some data is present in the buffer.
#define  pBIOCSENDPACKETSNOSYNC 9032	///< IOCTL code: Send a buffer containing multiple packets to the network, ignoring the timestamps associated with the packets.
#define  pBIOCSENDPACKETSSYNC 9033		///< IOCTL code: Send a buffer containing multiple packets to the network, respecting the timestamps associated with the packets.
#define  pBIOCSETDUMPLIMITS 9034		///< IOCTL code: Set the dump file limits. See the PacketSetDumpLimits() function.
#define  pBIOCISDUMPENDED 7411			///< IOCTL code: Get the status of the kernel dump process. See the PacketIsDumpEnded() function.

#define  pBIOCSTIMEZONE 7471			///< IOCTL code: set time zone. Used in Win9x only.


//Network type structure
typedef struct NetType
{
	UINT LinkType;	///< The MAC of the current network adapter (see function PacketGetNetType() for more information)
	ULONGLONG LinkSpeed;	///< The speed of the network in bits per second
}NetType;


// Alignment macros.  Packet_WORDALIGN rounds up to the next 
// even multiple of Packet_ALIGNMENT. 
#define Packet_ALIGNMENT sizeof(int)
#define Packet_WORDALIGN(x) (((x)+(Packet_ALIGNMENT-1))&~(Packet_ALIGNMENT-1))



struct bpf_program {
	UINT bf_len;
	struct bpf_insn *bf_insns;
};
struct bpf_insn {
	USHORT	code;
	UCHAR 	jt;
	UCHAR 	jf;
	int k;
};
struct bpf_stat {
	UINT bs_recv;		///< Number of packets that the driver received from the network adapter 
	///< from the beginning of the current capture. This value includes the packets 
	///< lost by the driver.
	UINT bs_drop;		///< number of packets that the driver lost from the beginning of a capture. 
	///< Basically, a packet is lost when the the buffer of the driver is full. 
	///< In this situation the packet cannot be stored and the driver rejects it.
	UINT ps_ifdrop;		///< drops by interface. XXX not yet supported
	UINT bs_capt;		///< number of packets that pass the filter, find place in the kernel buffer and
	///< thus reach the application.
};

struct bpf_hdr {
	struct timeval	bh_tstamp;		///<time stamp 
	UINT	bh_caplen;				///<length of captured portion 
	UINT	bh_datalen;				///<original length of packet
	USHORT		bh_hdrlen;			///<length of bpf header (this struct plus alignment padding)
};


#define ADAPTER_NAME_LENGTH 256 + 12	///<  Maximum length for the name of an adapter. The value is the same used by the IP Helper API.
#define ADAPTER_DESC_LENGTH 128			///<  Maximum length for the description of an adapter. The value is the same used by the IP Helper API.
#define MAX_MAC_ADDR_LENGTH 8			///<  Maximum length for the link layer address of an adapter. The value is the same used by the IP Helper API.
#define MAX_NETWORK_ADDRESSES 16		///<  Maximum length for the link layer address of an adapter. The value is the same used by the IP Helper API.


typedef struct WAN_ADAPTER_INT WAN_ADAPTER; ///< Describes an opened wan (dialup, VPN...) network adapter using the NetMon API
typedef WAN_ADAPTER *PWAN_ADAPTER; ///< Describes an opened wan (dialup, VPN...) network adapter using the NetMon API

#define INFO_FLAG_NDIS_ADAPTER		0	///< Flag for ADAPTER_INFO: this is a traditional ndis adapter
#define INFO_FLAG_NDISWAN_ADAPTER	1	///< Flag for ADAPTER_INFO: this is a NdisWan adapter
#define INFO_FLAG_DAG_CARD			2	///< Flag for ADAPTER_INFO: this is a DAG card
#define INFO_FLAG_DAG_FILE			6	///< Flag for ADAPTER_INFO: this is a DAG file


#define        DOSNAMEPREFIX   TEXT("Packet_")
#define        MAX_LINK_NAME_LENGTH   64
#define        NMAX_PACKET 65535  

typedef struct _ADAPTER  { 
	HANDLE hFile;				///< \internal Handle to an open instance of the NPF driver.
	CHAR  SymbolicLink[MAX_LINK_NAME_LENGTH]; ///< \internal A string containing the name of the network adapter currently opened.
	int NumWrites;				///< \internal Number of times a packets written on this adapter will be repeated 
	///< on the wire.
	HANDLE ReadEvent;			///< A notification event associated with the read calls on the adapter.
	///< It can be passed to standard Win32 functions (like WaitForSingleObject
	///< or WaitForMultipleObjects) to wait until the driver's buffer contains some 
	///< data. It is particularly useful in GUI applications that need to wait 
	///< concurrently on several events. In Windows NT/2000 the PacketSetMinToCopy()
	///< function can be used to define the minimum amount of data in the kernel buffer
	///< that will cause the event to be signalled. 

	UINT ReadTimeOut;			///< \internal The amount of time after which a read on the driver will be released and 
	///< ReadEvent will be signaled, also if no packets were captured
	CHAR Name[ADAPTER_NAME_LENGTH];
	PWAN_ADAPTER pWanAdapter;
	UINT Flags;					///< Adapter's flags. Tell if this adapter must be treated in a different way, using the Netmon API or the dagc API.
#ifdef HAVE_DAG_API
	dagc_t *pDagCard;			///< Pointer to the dagc API adapter descriptor for this adapter
	PCHAR DagBuffer;			///< Pointer to the buffer with the packets that is received from the DAG card
	struct timeval DagReadTimeout;	///< Read timeout. The dagc API requires a timeval structure
	unsigned DagFcsLen;			///< Length of the frame check sequence attached to any packet by the card. Obtained from the registry
	DWORD DagFastProcess;		///< True if the user requests fast capture processing on this card. Higher level applications can use this value to provide a faster but possibly unprecise capture (for example, libpcap doesn't convert the timestamps).
#endif // HAVE_DAG_API
}  ADAPTER, *LPADAPTER;

typedef struct _PACKET {  
	HANDLE       hEvent;		///< \deprecated Still present for compatibility with old applications.
	OVERLAPPED   OverLapped;	///< \deprecated Still present for compatibility with old applications.
	PVOID        Buffer;		///< Buffer with containing the packets. See the PacketReceivePacket() for
	///< details about the organization of the data in this buffer
	UINT         Length;		///< Length of the buffer
	DWORD        ulBytesReceived;	///< Number of valid bytes present in the buffer, i.e. amount of data
	///< received by the last call to PacketReceivePacket()
	BOOLEAN      bIoComplete;	///< \deprecated Still present for compatibility with old applications.
}  PACKET, *LPPACKET;




//iphelper structs
typedef struct _MIB_TCPROW_EX
{
    DWORD	dwState;		// MIB_TCP_STATE_*
    DWORD	dwLocalAddr;
    DWORD	dwLocalPort;
    DWORD	dwRemoteAddr;
    DWORD	dwRemotePort;
	DWORD	dwProcessId;
} MIB_TCPROW_EX, *PMIB_TCPROW_EX;

typedef struct _MIB_TCPTABLE_EX
{
    DWORD			dwNumEntries;
    MIB_TCPROW_EX	table[ANY_SIZE];
} MIB_TCPTABLE_EX, *PMIB_TCPTABLE_EX;

typedef struct _MIB_UDPROW_EX
{
    DWORD	dwLocalAddr;
    DWORD	dwLocalPort;
	DWORD	dwProcessId;
} MIB_UDPROW_EX, *PMIB_UDPROW_EX;

typedef struct _MIB_UDPTABLE_EX
{
    DWORD			dwNumEntries;
    MIB_UDPROW_EX	table[ANY_SIZE];
} MIB_UDPTABLE_EX, *PMIB_UDPTABLE_EX;
/*
DWORD
WINAPI
AllocateAndGetTcpExTableFromStack(
	OUT PMIB_TCPTABLE_EX	*pTcpTableEx,
	IN BOOL					bOrder,
	IN HANDLE				hAllocHeap,
	IN DWORD				dwAllocFlags,
	IN DWORD				dwProtocolVersion; // 2 - TCP, 23 - TCPv6 (size of *pTcpTableEx must be 56!)
);
*/
typedef DWORD (WINAPI *PROCALLOCATEANDGETTCPEXTABLEFROMSTACK)(PMIB_TCPTABLE_EX*,BOOL,HANDLE,DWORD,DWORD);

/*
DWORD
WINAPI
AllocateAndGetUdpExTableFromStack(
	OUT PMIB_UDPTABLE_EX	*pUdpTable,
	IN BOOL					bOrder,
	IN HANDLE				hAllocHeap,
	IN DWORD				dwAllocFlags,
	IN DWORD				dwProtocolVersion; // 2 - UDP, 23 - UDPv6 (size of *pUdpTable must be 28!)
);
*/
typedef DWORD (WINAPI *PROCALLOCATEANDGETUDPEXTABLEFROMSTACK)(PMIB_UDPTABLE_EX*,BOOL,HANDLE,DWORD,DWORD);

// IP header
#pragma pack(push, 1) //The IP_HEADER and ICMP_HEADER should be alligned on 1 byte boundaries
typedef struct tagIP_HEADER 
{
	unsigned int h_len:4;          // length of the header
	unsigned int version:4;        // Version of IP
	unsigned char tos;             // Type of service
	unsigned short total_len;      // total length of the packet
	unsigned short ident;          // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char  ttl; 
	unsigned char proto;           // protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum
	unsigned int sourceIP;
	unsigned int destIP;
} IP_HEADER;
typedef IP_HEADER FAR* LPIP_HEADER;

// ICMP header
typedef struct tagICMP_HEADER 
{
  BYTE i_type;
  BYTE i_code; /* type sub code */
  USHORT i_cksum;
  USHORT i_id;
  USHORT i_seq;
  /* This is not the std header, but we reserve space for time */
  ULONG timestamp;
} ICMP_HEADER;
typedef ICMP_HEADER FAR* LPICMP_HEADER;
#pragma pack(pop)




#endif // _MSC_VER > 1000

#endif __IPSTRUCTS_H__

