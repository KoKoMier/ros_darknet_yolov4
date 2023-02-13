#ifndef LAKIBEAMUDP_H
#define LAKIBEAMUDP_H

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bimap.hpp>

#include <sstream>
#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;

/// 已使用声明：用来消除未使用变量报警，没有功能上面的意义
#ifndef USED
#define USED(x) ((void)x)
#endif

/// 调试接口：快速打印调试信息
#ifndef DEBUG
#define DEBUG(x) (cout << x << endl)
#endif

/// 取消字节对齐：下面定义的数据结构体与雷达的协议结构相互映射，不可以进行自动对齐字节
#define __autoalign__ __attribute__((packed))

/**
 *  为了统一代码风格，对部分数据类型进行了重新定义
 */
#ifndef i32_t
typedef signed int i32_t;
#endif
#ifndef u32_t
typedef unsigned int u32_t;
#endif
#ifndef u16_t
typedef unsigned short u16_t;
#endif
#ifndef u16_t
typedef unsigned short i16_t;
#endif
#ifndef u8_t
typedef unsigned char u8_t;
#endif
#ifndef c8_t
typedef char i8_t;
#endif

#ifndef udp_socket_t
typedef boost::asio::ip::udp::socket udp_socket_t;
#endif
#ifndef address_t
typedef boost::asio::ip::address address_t;
#endif
#ifndef service_t
typedef boost::asio::io_service service_t;
#endif
typedef boost::scoped_ptr<boost::thread> thread_t;
#ifndef service_work_t
typedef boost::scoped_ptr<service_t::work> service_work_t;
#endif
#ifndef endpoint_t
typedef boost::asio::ip::udp::endpoint udp_endpoint_t;
#endif
#ifndef mutex_t
typedef boost::mutex mutex_t;
#endif

/// UDP数据报的数据帧大小——42字节UDP数据头不用于数据解码 1206 = 1248-42
#define CONFIG_FRAME (1206)
/// UDP数据包数据帧深度——必须为2的倍数
#define CONFIG_FRAME_MAX (256)
/// UDP数据缓存二位数组的第1个索引下标最大值
#define CONFIG_FRAME_INDEX_MAX (CONFIG_FRAME_MAX - 1)

/// 最大角度值
#define CONFIG_DEGREE_MAX (36000)

/// UDP数据报一帧数据中数据块个数
#define CONFIG_UDP_BLOCKS (12)
/// 数据块中数据池的深度
#define CONFIG_BLOCK_DEPTHS (16)

/// 转换缓存数据块深度
#define CONFIG_CIRCLE_DOTS (3600)

/// 雷达超时时间设置——2s
#define CONFIG_LASER_DISCONNECT (5)
#define CONFIG_LASER_TIMER_HEART (100)

/// 重组数据包大小
#define CONFIG_MAX_REPACK (32)

/// 16位数据大小端转换
#define T16_H8(x) ((u16_t)((((u16_t)x) & ((u16_t)(0xFF00))) >> 8))
#define T16_L8(x) ((u16_t)((((u16_t)x) & ((u16_t)(0x00FF))) << 8))
#define BIG_TO_LITTLE_16(x) ((u16_t)(T16_H8(x) | T16_L8(x)))
/// 32位数据大小端转换
#define T32_HH8(x) ((u32_t)((((u32_t)x) & ((u32_t)(0xFF000000))) >> 24))
#define T32_HL8(x) ((u32_t)((((u32_t)x) & ((u32_t)(0x00FF0000))) >> 8))
#define T32_LH8(x) ((u32_t)((((u32_t)x) & ((u32_t)(0x0000FF00))) << 8))
#define T32_LL8(x) ((u32_t)((((u32_t)x) & ((u32_t)(0x000000FF))) << 24))
#define BIG_TO_LITTLE_32(x) ((u32_t)(T32_HH8(x) | T32_HL8(x) | T32_LH8(x) | T32_LL8(x)))

/** @struct depth_t 雷达元数据包——最小数据包单元
 *   @brief  雷达深度数据的最小包裹
 */
#pragma pack(push)
#pragma pack(1)
struct depth_t
{
    u16_t distance0; ///< 深度数据
    u8_t rssi0;      ///< 回波强度
    u16_t distance1;
    u8_t rssi1;
};

/** @struct little_pack_t UDP数据包封装最小结构单元
 *   @brief  一帧雷达深度数据的数据结构的最小单元
 */
struct little_pack_t
{
    u16_t head;                         ///< 数据头
    u16_t azimuth;                      ///< 测距数据的角度值
    depth_t depth[CONFIG_BLOCK_DEPTHS]; ///< 深度数据点位包
};

/** @struct udp_pack_t UDP数据报一帧数据结构
 *   @brief  一帧UDP数据包的数据结构
 */
struct udp_pack_t
{
    little_pack_t depths[CONFIG_UDP_BLOCKS]; ///< 深度数据单元
    u32_t timestamp;                         ///< 时间戳
    u16_t factory;                           ///< 工厂信息
};

/** @struct cicle_pack_t UDP成品数据结构
 *   @brief  重新封装的深度数据包最小单元
 */
struct cicle_pack_t
{
	u32_t timestamp; ///< 时间戳
	u16_t angle;     ///< 角度值
	u16_t distance;  ///< 深度数据
	u8_t rssi;       ///< 回波强度
};

/** @struct repark_t 深度数据块，一块包含雷达一周的深度数据
 *   @brief  一帧代表一块深度数据
 */
struct repark_t
{
    u32_t interval;                            ///< 间隔时间
    u16_t maxdots;                             ///< 点位个数
    cicle_pack_t dotcloud[CONFIG_CIRCLE_DOTS]; ///< 深度数据点云
};
#pragma pack(pop)

class LakiBeamUDP
{

public:
    /*!
            构造函数
            \param local_ip     本地ip
            \param local_port   本地端口号
            \param laser_ip     雷达ip
            \param laser_port   雷达端口号
        */
    LakiBeamUDP(string local_ip, string local_port, string laser_ip, string laser_port);
    /*!
            析构函数
        */
    ~LakiBeamUDP();
    /*!
            同步重新组包功能
            \param pack   用户包裹出口
        */
    void sync_get_repackedpack(repark_t &pack);
	/*!
			取出数据包裹 返回true那么立即返回数据
			\param pack   用户包裹出口
		*/
	bool get_repackedpack(repark_t &pack);

private:
    string local_ip;
    string local_port;
    string laser_ip;
    string laser_port;

    thread_t receive_thd;
    thread_t udprepack_thd;

    mutex_t thd_mutex;
	bool everyThingOK;
    volatile u32_t dbmain;
	volatile u32_t dbmain_used;
	udp_pack_t doublebuffer[CONFIG_FRAME_MAX];

	volatile u32_t urpmain;
	volatile u32_t urpmain_used;
	repark_t udprepack[CONFIG_MAX_REPACK];

    char buffff[CONFIG_FRAME];
    udp_socket_t* socket;
    boost::asio::io_service io_servicess;
    udp_endpoint_t *local_ep;
    udp_endpoint_t *laser_ep;

private:
    /*!
            接受UDP数据包线程
        */
    void receive_thread(void);
    /*!
            UDP拆包组包线程
        */
    void udprepack_thread(void);
    /*!
            填充没有起始包的包裹（一次需要2包）
            \param pack   包裹入口数组
        */
    void fill_nostart(udp_pack_t pack[2]);
    /*!
            填充有起始包的包裹（一次需要2包）
            \param pack   包裹入口数组
        */
    void fill_havestart(udp_pack_t pack[2], u32_t start);
    void on_read(const boost::system::error_code& err, std::size_t read_bytes);
};

#endif // LAKIBEAMUDP_H
