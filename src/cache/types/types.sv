// Converted SystemVerilog type definitions based on types.hpp

// Memory constants
`define MAX_MEMORY_SPACE (512*1024*1024)

// Typedef equivalents in SystemVerilog
typedef logic [31:0] t_BlockOffset;
typedef logic [31:0] t_Size;
typedef logic [31:0] t_Index;
typedef logic [15:0] t_Bank;
typedef logic [15:0] t_Way;
typedef logic [31:0] t_Slice;
typedef logic [31:0] t_CacheLevel;
typedef logic [31:0] t_Stack;
typedef logic [31:0] t_Credit;
typedef logic [31:0] t_PortNo;
typedef logic [63:0] t_Tag;
typedef logic [63:0] t_BlockAddress;
typedef logic [63:0] t_Address;
typedef logic [63:0] t_VirtualAddress;
typedef logic [63:0] t_PhysicalAddress;
typedef logic [63:0] t_TraceLength;
typedef logic [63:0] t_TraceShuffler;
typedef logic [63:0] t_TraceLocality;
typedef logic [63:0] t_TraceStride;
typedef logic [7:0] t_Data[];  // byte = 8 bits
typedef logic [63:0] t_Counter;
typedef logic [63:0] t_Time;
typedef logic [31:0] t_CoreID;
typedef logic [31:0] t_SHRID;
typedef logic [63:0] t_CompressionData;
typedef logic t_BinaryFlag;  // or `bit` if no X/Z needed


// Macro constants
`define M_TAG_MAX 64'hFFFF_FFFF_FFFF_FFFF
`define M_CORE_MAX 32'hFFFF_FFFF
`define M_OFFSET_MAX 32'hFFFF_FFFF
`define M_INDEX_MAX 32'hFFFF_FFFF
`define M_WAY_MAX 16'hFFFF
`define M_BANK_MAX 16'hFFFF
`define M_SLICE_MAX 16'hFFFF
`define M_INSTALL_LEVEL_MAX 16'hFFFF
`define M_FU_PORT 0
`define M_ICACHE_PORT 0
`define M_DCACHE_PORT 0
`define M_MAX_NODE 32'hFFFF_FFFF
`define M_CORE_NOC_LEVEL 1
`define M_CACHE_NOC_LEVEL 2

// Enums
typedef enum int {
  E_REQQ_PORT = 0,
  E_PORT_REQQ = 1,
  E_NUM_PRIORITY_SCHED
} t_RequestQueuePriority;

typedef enum int {
  E_INSTRUCTION = 0,
  E_DATA = 1,
  E_SMC = 2,
  E_NOLINETYPE = 3,
  E_NUM_LINETYPE
} t_LineType;

typedef enum int {
	E_INVALID = 0,
	E_DATA_WAIT = 1,
	E_EXCLUSIVE = 2,
	E_SHD_EX = 3,
	E_SHARED = 4,
	E_MODIFIED = 5,
	E_NUM_STATES
} t_CacheState;

typedef enum int {
	E_PIPT = 0,
	E_VIPT = 1,
	E_VIVT = 2,
	E_NUM_ADDR_TYPE
} t_AddressType;

typedef enum {
	E_CORE_READ = 0,
	E_CORE_WRITE = 1,
	E_PROBE_REQ = 2,
	E_PROBE_RESP = 3,
	E_FILL = 4,
	E_PREFETCH_RECV = 5,
	E_EVICT = 6,
	E_NUM_RECVQ_TYPE
} t_RequestType;

typedef enum {
	E_MISS = 0,
	E_VICTIM = 1,
	E_PROBE_REQ_SEND = 2,
	E_PROBE_RESP_SEND = 3,
	E_READ_RESP = 4,
	E_PREFETCH_SEND = 5,
	E_NUM_SENDQ_TYPE
} t_ResponseType;

typedef enum int {
	E_READ_SHR = 0,
	E_VICTIM_BUFFER = 1,
	E_PROBE_SHR = 2,
	E_ISSUE_BUFFER = 3,
	E_NUM_SHR_TYPE
} t_SHRType;

typedef enum int {
	E_REQ_Q = 0,
	E_RESP_Q = 1,
	E_ISSUE_Q = 2,
	E_NUM_Q_TYPES
} t_QueueType;

typedef enum int {
	E_C2X_REQ = 0,
	E_C2X_RESP = 1,
	E_NO_C2X
} t_ChangeToXType;

typedef struct packed	{
	t_RequestType m_requestType;
	t_ResponseType m_responseType;
	t_Size m_portIndex;
	t_Credit m_packetIndex;
	t_QueueType m_queueType;
} c_PacketQueueLocation;

typedef enum int {
	E_FIXED_LATENCY_DATA_READ = 0,
	E_FIXED_LATENCY_DATA_WRITE = 1,
	E_NUM_FIXED_LATENCY_REQTYPE
} t_FixedLatencyRequestType;

typedef enum int {
	E_PRB_NOTHING = 0,
	E_PRB_NOCHANGE = 1,
	E_PRB_INVALIDATE = 2,
	E_PRB_SHARED = 3,
	E_NUM_PROBETYPE
} t_ProbeType;

typedef enum int {
	E_METADATA = 0,
	E_BYPASS = 1
} t_RequestSchedulerType;

typedef enum int {
	E_RCS_IDLE = 0,
	E_RCS_FETCHED = 1,
	E_RCS_DECODED = 2,
	E_RCS_ISSUED = 3,
	E_RCS_EXECUTED = 4,
	E_RCS_RETIRED = 5,
	E_NUM_RCS_STATES
} t_RequestControllerState;

typedef enum int {
	E_LUS_HIT = 0,
	E_LUS_MISS = 1,
	E_LUS_HIT_NO_PERM = 3,
	E_LUS_DIRECTORY_HIT = 4,
	E_LUS_DIRECTORY_MISS = 5,
	E_LUS_NA
} t_LookUpStatus;

typedef enum int {
	E_MCS_IDLE = 0,
	E_MCS_TAG_READ_DONE = 1,
	E_MCS_DATA_READ_DONE = 2,
	E_MCS_DATA_WRITE_DONE = 3,
	E_MCS_TAG_WRITE_DONE = 4,
	E_MCS_RETIRED = 5,
	E_NUM_MCS_STATES
} t_MetadataControllerState;
// ... (repeat pattern for all enums in the original header)

// Structs
typedef struct packed {
  t_RequestType m_requestType;
  t_ResponseType m_responseType;
  t_Size m_portIndex;
  t_Credit m_packetIndex;
  t_QueueType m_queueType;
} c_PacketQueueLocation;

typedef enum int t_StorageControllerState	{
	E_SCS_IDLE = 0,
	E_SCS_READ_WAIT = 1,
	E_SCS_WRITE_WAIT = 2
};

typedef enum int t_StateRequestType	{
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

typedef enum int {
	E_DATA_READ = 0,
	E_DATA_WRITE = 1,
	E_TAG_WRITE = 2,
	E_METADATA_RESP = 3,
	E_NUM_ISSUE_TYPE
} t_IssueType;

typedef enum int {
	E_STATERESP_FAIL = 0,
	E_STATERESP_DATA = 1,
	E_STATERESP_ACK = 2,
	E_STATERESP_MISS = 3,
	E_STATERESP_DATA_WB = 4,
	E_NUM_STATE_RESPTYPE
} t_StateResponseType;

typedef enum int {
	E_REPL_POLICY_LRU = 0,
	E_REPL_POLICY_FIFO_ALLOC = 1,
	E_REPL_POLICY_FIFO_FILL = 2,
	E_REPL_POLICY_RANDOM = 3,
	E_NUM_REPL_POLICY
} t_ReplacementPolicy;

typedef enum int {
	E_INDEXING_NOHASH = 0,
	E_INDEXING_HASH = 1,
	E_NUM_INDEXING_POLICY
} t_IndexingPolicy;

typedef enum int {
	E_COMPRESS_NO = 0,
	E_COMPRESS_ZERO = 1,
	E_COMPRESS_FVC = 2,
	E_COMPRESS_BDI_ALGO = 3,
	E_COMPRESS_FPC_ALGO = 4,
	E_COMPRESS_BEST = 5,
	E_NUM_COMPRESS_ALGO
} t_CompressionAlgorithm;

typedef enum int {
   E_SWITCH = 0,
   E_REPEATER = 1,
   E_DUMMY = 2,
   E_MAX_DEVICES = 3
} t_DeviceType;

typedef enum int {
    E_PREV = 0,
    E_NEXT = 1,
    E_MAX_DIR_REPEATER = 2
} t_RepeaterDirection;

typedef enum int {
	E_PRIVATE_CACHE = 0,
	E_SHARED_CACHE = 1,
	E_MEMORY = 2,
	E_MAX_SOURCE_DESTINATION_TYPE = 3
} t_SourceDestinationType;

typedef enum int {
	E_PE = 0,
	E_MM = 1,
	E_EAST = 2,
	E_WEST = 3,
	E_NORTH = 4,
	E_SOUTH = 5,
	E_UP = 6,
	E_DOWN = 7,

	E_MAX_DIR_SWITCH = 8,
} t_SwitchDirection;

typedef enum int {
    E_XAXIS = 0,
    E_YAXIS = 1,
    E_ZAXIS = 2
} t_AxisType;

typedef logic [31:0] t_NetworkChannel;
typedef logic [31:0] t_Layers;
typedef logic [31:0] t_PacketId;
typedef logic [31:0] t_Axis;
typedef logic [31:0] t_NodeId;

typedef enum int {
    E_SHORTESTPATHALGO = 1
} t_RoutingAlgorithm;