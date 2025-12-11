#include <limits.h>

#ifndef TYPES_H
#define TYPES_H

#define MAX_MEMORY_SPACE 512*1024*1024ULL

typedef unsigned int t_BlockOffset;
typedef unsigned int t_Size;
typedef unsigned int t_Index;
typedef unsigned short t_Bank;
typedef unsigned short t_Way;
typedef unsigned int t_Slice;
typedef unsigned int t_CacheLevel;
typedef unsigned int t_Stack;
typedef unsigned int t_Credit;
typedef unsigned int t_PortNo;
typedef unsigned long long t_Tag;
typedef unsigned long long t_BlockAddress;
typedef unsigned long long t_Address;
typedef unsigned long long t_VirtualAddress;
typedef unsigned long long t_PhysicalAddress;
typedef unsigned long long t_TraceLength;
typedef unsigned long long t_TraceShuffler;
typedef unsigned long long t_TraceLocality;
typedef unsigned long long t_TraceStride;
typedef unsigned char* t_Data;
typedef unsigned long long t_Counter;
typedef unsigned long long t_Time;
typedef unsigned int t_CoreID;
typedef unsigned int t_SHRID;
typedef unsigned long long t_CompressionData;
typedef bool t_BinaryFlag;

#define M_TAG_MAX ULLONG_MAX
#define M_CORE_MAX UINT_MAX
#define M_OFFSET_MAX UINT_MAX
#define M_INDEX_MAX UINT_MAX
#define M_WAY_MAX USHRT_MAX
#define M_BANK_MAX USHRT_MAX
#define M_SLICE_MAX USHRT_MAX
#define M_INSTALL_LEVEL_MAX USHRT_MAX
#define M_FU_PORT 0
#define M_ICACHE_PORT 0
#define M_DCACHE_PORT 0
#define M_MAX_NODE UINT_MAX
#define M_CORE_NOC_LEVEL 1
#define M_CACHE_NOC_LEVEL 2

enum t_RequestQueuePriority	{
	E_REQQ_PORT = 0,
	E_PORT_REQQ = 1,
	E_NUM_PRIORITY_SCHED
};

enum t_LineType	{
	E_INSTRUCTION = 0,
	E_DATA = 1,
	E_SMC = 2,
	E_NOLINETYPE = 3,
	E_NUM_LINETYPE
};

enum t_CacheState	{
	E_INVALID = 0,
	E_DATA_WAIT = 1,
	E_EXCLUSIVE = 2,
	E_SHD_EX = 3,
	E_SHARED = 4,
	E_MODIFIED = 5,
	E_NUM_STATES
};

enum t_AddressType	{
	E_PIPT = 0,
	E_VIPT = 1,
	E_VIVT = 2,
	E_NUM_ADDR_TYPE
};

enum t_RequestType	{
	E_CORE_READ = 0,
	E_CORE_WRITE = 1,
	E_PROBE_REQ = 2,
	E_PROBE_RESP = 3,
	E_FILL = 4,
	E_PREFETCH_RECV = 5,
	E_EVICT = 6,
	E_NUM_RECVQ_TYPE
};

enum t_ResponseType	{
	E_MISS = 0,
	E_VICTIM = 1,
	E_PROBE_REQ_SEND = 2,
	E_PROBE_RESP_SEND = 3,
	E_READ_RESP = 4,
	E_PREFETCH_SEND = 5,
	E_NUM_SENDQ_TYPE
};

enum t_SHRType	{
	E_READ_SHR = 0,
	E_VICTIM_BUFFER = 1,
	E_PROBE_SHR = 2,
	E_ISSUE_BUFFER = 3,
	E_NUM_SHR_TYPE
};

enum t_QueueType	{
	E_REQ_Q = 0,
	E_RESP_Q = 1,
	E_ISSUE_Q = 2,
	E_NUM_Q_TYPES
};

enum t_ChangeToXType	{
	E_C2X_REQ = 0,
	E_C2X_RESP = 1,
	E_NO_C2X
};

struct c_PacketQueueLocation	{
	t_RequestType m_requestType;
	t_ResponseType m_responseType;
	t_Size m_portIndex;
	t_Credit m_packetIndex;
	t_QueueType m_queueType;
};

enum t_FixedLatencyRequestType	{
	E_FIXED_LATENCY_DATA_READ = 0,
	E_FIXED_LATENCY_DATA_WRITE = 1,
	E_NUM_FIXED_LATENCY_REQTYPE
};



enum t_ProbeType	{
	E_PRB_NOTHING = 0,
	E_PRB_NOCHANGE = 1,
	E_PRB_INVALIDATE = 2,
	E_PRB_SHARED = 3,
	E_NUM_PROBETYPE
};

enum t_RequestSchedulerType {
	E_METADATA = 0,
	E_BYPASS = 1
};

enum t_RequestControllerState	{
	E_RCS_IDLE = 0,
	E_RCS_FETCHED = 1,
	E_RCS_DECODED = 2,
	E_RCS_ISSUED = 3,
	E_RCS_EXECUTED = 4,
	E_RCS_RETIRED = 5,
	E_NUM_RCS_STATES
};

enum t_LookUpStatus	{
	E_LUS_HIT = 0,
	E_LUS_MISS = 1,
	E_LUS_HIT_NO_PERM = 3,
	E_LUS_DIRECTORY_HIT = 4,
	E_LUS_DIRECTORY_MISS = 5,
	E_LUS_NA
};

enum t_MetadataControllerState	{
	E_MCS_IDLE = 0,
	E_MCS_TAG_READ_DONE = 1,
	E_MCS_DATA_READ_DONE = 2,
	E_MCS_DATA_WRITE_DONE = 3,
	E_MCS_TAG_WRITE_DONE = 4,
	E_MCS_RETIRED = 5,
	E_NUM_MCS_STATES
};

enum t_StorageControllerState	{
	E_SCS_IDLE = 0,
	E_SCS_READ_WAIT = 1,
	E_SCS_WRITE_WAIT = 2
};

enum t_StateRequestType	{
	E_SRT_RESERVE = 0,
	E_SRT_READ = 1,
	E_SRT_WRITE = 2,
	E_SRT_PROBE_SHD = 4,
	E_SRT_PROBE_INV = 5,
	E_SRT_EVICT = 6,
	E_SRT_EX_PER = 7,
	E_SRT_FILL = 8,
	E_NUM_STATE_REQTYPE
};

enum t_IssueType	{
	E_DATA_READ = 0,
	E_DATA_WRITE = 1,
	E_TAG_WRITE = 2,
	E_METADATA_RESP = 3,
	E_NUM_ISSUE_TYPE
};

enum t_StateResponseType {
	E_STATERESP_FAIL = 0,
	E_STATERESP_DATA = 1,
	E_STATERESP_ACK = 2,
	E_STATERESP_MISS = 3,
	E_STATERESP_DATA_WB = 4,
	E_NUM_STATE_RESPTYPE
};

enum t_ReplacementPolicy	{
	E_REPL_POLICY_LRU = 0,
	E_REPL_POLICY_FIFO_ALLOC = 1,
	E_REPL_POLICY_FIFO_FILL = 2,
	E_REPL_POLICY_RANDOM = 3,
	E_NUM_REPL_POLICY
};

enum t_IndexingPolicy	{
	E_INDEXING_NOHASH = 0,
	E_INDEXING_HASH = 1,
	E_NUM_INDEXING_POLICY
};

enum t_CompressionAlgorithm	{
	E_COMPRESS_NO = 0,
	E_COMPRESS_ZERO = 1,
	E_COMPRESS_FVC = 2,
	E_COMPRESS_BDI_ALGO = 3,
	E_COMPRESS_FPC_ALGO = 4,
	E_COMPRESS_BEST = 5,
	E_NUM_COMPRESS_ALGO
};

enum t_DeviceType {
   E_SWITCH = 0,
   E_REPEATER = 1,
   E_DUMMY = 2,
   E_MAX_DEVICES = 3
};

enum t_RepeaterDirection {
    E_PREV = 0,
    E_NEXT = 1,
    E_MAX_DIR_REPEATER = 2
};

enum t_SourceDestinationType {
	E_PRIVATE_CACHE = 0,
	E_SHARED_CACHE = 1,
	E_MEMORY = 2,
	E_MAX_SOURCE_DESTINATION_TYPE = 3
};

enum t_SwitchDirection  {
	E_PE = 0,
	E_MM = 1,
	E_EAST = 2,
	E_WEST = 3,
	E_NORTH = 4,
	E_SOUTH = 5,
	E_UP = 6,
	E_DOWN = 7,

	E_MAX_DIR_SWITCH = 8,
};

enum t_AxisType{
    E_XAXIS = 0,
    E_YAXIS = 1,
    E_ZAXIS = 2
};

typedef unsigned int t_NetworkChannel;
typedef unsigned int t_Layers;
typedef unsigned int t_PacketId;
typedef unsigned int t_Axis;
typedef unsigned int t_NodeId;


enum t_RoutingAlgorithm{
    E_SHORTESTPATHALGO = 1
};

#endif
