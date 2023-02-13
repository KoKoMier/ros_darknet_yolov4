#include "LakiBeamUDP.h"

LakiBeamUDP::LakiBeamUDP(string local_ip, string local_port, string laser_ip, string laser_port)
{
    this->local_ip = local_ip;
    this->local_port = local_port;
    this->laser_ip = laser_ip;
    this->laser_port = laser_port;

    dbmain = 0;
    dbmain_used = 0;
    urpmain = 0;
    urpmain_used = 0;

    char *temp = reinterpret_cast<char *>(udprepack);
    memset(temp, 0, CONFIG_MAX_REPACK * sizeof(repark_t));

    local_ep = new udp_endpoint_t(address_t::from_string("0.0.0.0"), atoi(local_port.c_str()));
    laser_ep = new udp_endpoint_t(address_t::from_string(laser_ip), atoi(laser_port.c_str()));
    socket = new udp_socket_t(io_servicess);
    socket->open(local_ep->protocol());
    socket->set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket->bind(*local_ep);
    socket->async_receive_from(boost::asio::buffer(buffff, CONFIG_FRAME), *laser_ep, boost::bind(&LakiBeamUDP::on_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    udprepack_thd.reset(new boost::thread(boost::bind(&LakiBeamUDP::udprepack_thread, this)));
    receive_thd.reset(new boost::thread(boost::bind(&LakiBeamUDP::receive_thread, this)));
}

void LakiBeamUDP::receive_thread()
{
    while (1)
    {
        try {
            io_servicess.run();
            boost::this_thread::interruption_point();
        }
        catch (boost::system::error_code& err)
        {
            break;
        }
    }
}

LakiBeamUDP::~LakiBeamUDP()
{
    io_servicess.stop();

    receive_thd->interrupt();
    receive_thd->join();

    delete socket;
    delete local_ep;
    delete laser_ep;

    udprepack_thd->interrupt();
    udprepack_thd->join();
}

void LakiBeamUDP::sync_get_repackedpack(repark_t &pack)
{
    while (urpmain < 2)
    {
		
    }
    while (urpmain <= (urpmain_used + 1))
    {

    }
    urpmain_used = urpmain - 1;

    pack = udprepack[(urpmain_used) % CONFIG_MAX_REPACK];
}

bool LakiBeamUDP::get_repackedpack(repark_t &pack)
{
	bool result = true;

	if ((urpmain < 2)||(urpmain <= (urpmain_used + 1)))
	{
		result = false;
	}
	else
	{
		urpmain_used = urpmain - 1;
	}

	if (result == true)
	{
        pack = udprepack[(urpmain_used) % CONFIG_MAX_REPACK];
	}

	return result;
}

void LakiBeamUDP::on_read(const boost::system::error_code& err, std::size_t read_bytes)
{
    if (!err)
    {
        char* temp;
        if (read_bytes == CONFIG_FRAME)
        {
            temp = reinterpret_cast<char*>(&(doublebuffer[dbmain % CONFIG_FRAME_MAX]));
            memcpy(temp, buffff, CONFIG_FRAME);
            dbmain++;
        }
    }
    socket->async_receive_from(boost::asio::buffer(buffff, CONFIG_FRAME), *laser_ep, boost::bind(&LakiBeamUDP::on_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void LakiBeamUDP::udprepack_thread()
{
    while (1)
    {
        try
        {
            bool goon = true;
            udp_pack_t doublebuffertemp[2];
            {
                if ((dbmain - dbmain_used) >= 2)
                {
                    char *source1 = reinterpret_cast<char *>(&doublebuffertemp[0]);
                    char *source2 = reinterpret_cast<char *>(&doublebuffertemp[1]);
                    char *from1 = reinterpret_cast<char *>(&(doublebuffer[dbmain_used % CONFIG_FRAME_MAX]));
                    char *from2 = reinterpret_cast<char *>(&(doublebuffer[(dbmain_used + 1) % CONFIG_FRAME_MAX]));
                    u32_t realindex = dbmain_used % CONFIG_FRAME_MAX;
                    if (realindex == CONFIG_FRAME_INDEX_MAX)
                    {
                        memcpy(source1, from1, CONFIG_FRAME);
                        memcpy(source2, from2, CONFIG_FRAME);
                    }
                    else
                    {
                        memcpy(source1, from1, CONFIG_FRAME * 2);
                    }
                    dbmain_used++;
                }
                else
                {
                    goon = false;
                }
            }

            if (goon == true)
            {
                if ((doublebuffertemp[0].factory == 0x4037) && (doublebuffertemp[1].factory == 0x4037))
                {
                    // 寻找0位包裹
                    u32_t start_index = 0xFFFFFFFF;
                    for (int i = 0; i < CONFIG_UDP_BLOCKS; i++)
                    {
                        if (doublebuffertemp[0].depths[i].azimuth == 0)
                        {
                            start_index = i;
                        }
                    }
                    // 0位数据包
                    if (start_index != 0xFFFFFFFF)
                    {
                        fill_havestart(doublebuffertemp, start_index);
                    }
                    // 非0位数据包
                    else
                    {
                        fill_nostart(doublebuffertemp);
                    }
                }
                else
                {
                    //                    DEBUG(doublebuffertemp[1].factory);
                    //                    DEBUG(doublebuffertemp[0].factory);
                }
            }
            boost::this_thread::interruption_point();
			if ((dbmain - dbmain_used) < 2)
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(2));
			}
        }
        catch (...)
        {
            break;
        }
    }
}

void LakiBeamUDP::fill_nostart(udp_pack_t pack[2])
{
    (void)pack;
    u32_t start_time = pack[0].timestamp;
    u32_t end_time = pack[1].timestamp;

    u16_t tempdots = udprepack[urpmain % CONFIG_MAX_REPACK].maxdots;
    u32_t div_time = (end_time - start_time) / CONFIG_BLOCK_DEPTHS / CONFIG_UDP_BLOCKS;

    for (int i = 0; i < CONFIG_UDP_BLOCKS; i++)
    {
        if (pack[0].depths[i].head == 0xEEFF)
        {
            if (i < (CONFIG_UDP_BLOCKS - 1))
            {
                u16_t start_angle = pack[0].depths[i].azimuth;
                u16_t end_angle = pack[0].depths[i + 1].azimuth;
                u16_t div_angle;
                if (end_angle >= start_angle)
                {
                    div_angle = (end_angle - start_angle) / CONFIG_BLOCK_DEPTHS;
                }
                else
                {
                    div_angle = (CONFIG_DEGREE_MAX - start_angle + end_angle) / CONFIG_BLOCK_DEPTHS;
                }

                for (int j = 0; j < CONFIG_BLOCK_DEPTHS; j++)
                {
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].timestamp = start_time;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].angle = start_angle;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].distance = pack->depths[i].depth[j].distance0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].rssi = pack->depths[i].depth[j].rssi0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].interval += div_time;
                    tempdots++;
                    if ((start_angle + div_angle) >= CONFIG_DEGREE_MAX)
                    {
                        start_angle = start_angle + div_angle - CONFIG_DEGREE_MAX;
                    }
                    else
                    {
                        start_angle += div_angle;
                    }
                    start_time += div_time;
                }
            }
            else
            {
                u16_t start_angle = pack[0].depths[i].azimuth;
                u16_t end_angle = pack[1].depths[0].azimuth;
                u16_t div_angle;
                if (end_angle >= start_angle)
                {
                    div_angle = (end_angle - start_angle) / CONFIG_BLOCK_DEPTHS;
                }
                else
                {
                    div_angle = (CONFIG_DEGREE_MAX - start_angle + end_angle) / CONFIG_BLOCK_DEPTHS;
                }
                for (int j = 0; j < CONFIG_BLOCK_DEPTHS; j++)
                {
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].timestamp = start_time;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].angle = start_angle;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].distance = pack->depths[i].depth[j].distance0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].rssi = pack->depths[i].depth[j].rssi0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].interval += div_time;
                    tempdots++;
                    if ((start_angle + div_angle) >= CONFIG_DEGREE_MAX)
                    {
                        start_angle = start_angle + div_angle - CONFIG_DEGREE_MAX;
                    }
                    else
                    {
                        start_angle += div_angle;
                    }
                    start_time += div_time;
                }
            }
        }
    }
    udprepack[urpmain % CONFIG_MAX_REPACK].maxdots = tempdots;
}

void LakiBeamUDP::fill_havestart(udp_pack_t pack[], u32_t start)
{
    // 第一包
    if (urpmain == 0)
    {
        urpmain++;
        urpmain_used++;

        udprepack[urpmain % CONFIG_MAX_REPACK].maxdots = 0;
        udprepack[urpmain % CONFIG_MAX_REPACK].interval = 0;

        (void)pack;

        u32_t start_time = pack[0].timestamp;
        u32_t end_time = pack[1].timestamp;

        u16_t tempdots = udprepack[urpmain % CONFIG_MAX_REPACK].maxdots;
        u32_t div_time = (end_time - start_time) / CONFIG_BLOCK_DEPTHS / CONFIG_UDP_BLOCKS;

        for (int i = start; i < CONFIG_UDP_BLOCKS; i++)
        {
            if (i < (CONFIG_UDP_BLOCKS - 1))
            {
                u16_t start_angle = pack[0].depths[i].azimuth;
                u16_t end_angle = pack[0].depths[i + 1].azimuth;
                u16_t div_angle;
                if (end_angle >= start_angle)
                {
                    div_angle = (end_angle - start_angle) / CONFIG_BLOCK_DEPTHS;
                }
                else
                {
                    div_angle = (CONFIG_DEGREE_MAX - start_angle + end_angle) / CONFIG_BLOCK_DEPTHS;
                }

                for (int j = 0; j < CONFIG_BLOCK_DEPTHS; j++)
                {

                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].timestamp = start_time;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].angle = start_angle;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].distance = pack->depths[i].depth[j].distance0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].rssi = pack->depths[i].depth[j].rssi0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].interval += div_time;
                    tempdots++;
                    if ((start_angle + div_angle) >= CONFIG_DEGREE_MAX)
                    {
                        start_angle = start_angle + div_angle - CONFIG_DEGREE_MAX;
                    }
                    else
                    {
                        start_angle += div_angle;
                    }
                    start_time += div_time;
                }
            }
            else
            {
                u16_t start_angle = pack[0].depths[i].azimuth;
                u16_t end_angle = pack[1].depths[0].azimuth;
                u16_t div_angle;
                if (end_angle >= start_angle)
                {
                    div_angle = (end_angle - start_angle) / CONFIG_BLOCK_DEPTHS;
                }
                else
                {
                    div_angle = (CONFIG_DEGREE_MAX - start_angle + end_angle) / CONFIG_BLOCK_DEPTHS;
                }
                start_angle = start_angle + div_angle * (start + 1) * CONFIG_BLOCK_DEPTHS;
                for (int j = 0; j < CONFIG_BLOCK_DEPTHS; j++)
                {
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].timestamp = start_time;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].angle = start_angle;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].distance = pack->depths[i].depth[j].distance0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].rssi = pack->depths[i].depth[j].rssi0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].interval += div_time;
                    tempdots++;
                    if ((start_angle + div_angle) >= CONFIG_DEGREE_MAX)
                    {
                        start_angle = start_angle + div_angle - CONFIG_DEGREE_MAX;
                    }
                    else
                    {
                        start_angle += div_angle;
                    }
                    start_time += div_time;
                }
            }
        }
        udprepack[urpmain % CONFIG_MAX_REPACK].maxdots = tempdots;
    }
    // 其它0包
    else
    {
        {
            (void)pack;
            u32_t start_time = pack[0].timestamp;
            u32_t end_time = pack[1].timestamp;

            u16_t tempdots = udprepack[urpmain % CONFIG_MAX_REPACK].maxdots;
            u32_t div_time = (end_time - start_time) / CONFIG_BLOCK_DEPTHS / CONFIG_UDP_BLOCKS;

            udprepack[urpmain % CONFIG_MAX_REPACK].interval += div_time;

            for (u32_t i = 0; i < start; i++)
            {
                u16_t start_angle = pack[0].depths[i].azimuth;
                u16_t end_angle = pack[0].depths[i + 1].azimuth;
                u16_t div_angle;
                if (end_angle >= start_angle)
                {
                    div_angle = (end_angle - start_angle) / CONFIG_BLOCK_DEPTHS;
                }
                else
                {
                    div_angle = (CONFIG_DEGREE_MAX - start_angle + end_angle) / CONFIG_BLOCK_DEPTHS;
                }
                for (int j = 0; j < CONFIG_BLOCK_DEPTHS; j++)
                {
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].timestamp = start_time;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].angle = start_angle;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].distance = pack->depths[i].depth[j].distance0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].rssi = pack->depths[i].depth[j].rssi0;
                    udprepack[urpmain % CONFIG_MAX_REPACK].interval += div_time;
                    tempdots++;
                    if ((start_angle + div_angle) >= CONFIG_DEGREE_MAX)
                    {
                        start_angle = start_angle + div_angle - CONFIG_DEGREE_MAX;
                    }
                    else
                    {
                        start_angle += div_angle;
                    }
                    start_time += div_time;
                }
            }
			if((tempdots==1440)||(tempdots==3600))
			{
                bool lost = false;
                if (tempdots == 1440)
                {
                    for (int i = 180; i < (1440 - 17); i++)
                    {
                        if (udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[i+1].angle < udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[i].angle)
                        {
                            lost = true;
                        }
                    }
                }
                else
                {
                    for (int i = 450; i < (3600 - 17); i++)
                    {
                        if (udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[i + 1].angle < udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[i].angle)
                        {
                            lost = true;
                        }
                    }
                }

                if (lost == false)
                {
                    udprepack[urpmain % CONFIG_MAX_REPACK].interval /= tempdots;
                    udprepack[urpmain % CONFIG_MAX_REPACK].maxdots = tempdots;
                    urpmain++;
                }
			}
        }
        {
            udprepack[urpmain % CONFIG_MAX_REPACK].maxdots = 0;

            (void)pack;
            u32_t start_time = pack[0].timestamp;
            u32_t end_time = pack[1].timestamp;

            u16_t tempdots = udprepack[urpmain % CONFIG_MAX_REPACK].maxdots;
            u32_t div_time = (end_time - start_time) / CONFIG_BLOCK_DEPTHS / CONFIG_UDP_BLOCKS;

            for (u32_t i = start; i < CONFIG_UDP_BLOCKS; i++)
            {
                if (i < (CONFIG_UDP_BLOCKS - 1))
                {
                    u16_t start_angle = pack[0].depths[i].azimuth;
                    u16_t end_angle = pack[0].depths[i + 1].azimuth;
                    u16_t div_angle;
                    if (end_angle >= start_angle)
                    {
                        div_angle = (end_angle - start_angle) / CONFIG_BLOCK_DEPTHS;
                    }
                    else
                    {
                        div_angle = (CONFIG_DEGREE_MAX - start_angle + end_angle) / CONFIG_BLOCK_DEPTHS;
                    }
                    for (int j = 0; j < CONFIG_BLOCK_DEPTHS; j++)
                    {
                        udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].timestamp = start_time;
                        udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].angle = start_angle;
                        udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].distance = pack->depths[i].depth[j].distance0;
                        udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].rssi = pack->depths[i].depth[j].rssi0;
                        udprepack[urpmain % CONFIG_MAX_REPACK].interval += div_time;
                        tempdots++;
                        if ((start_angle + div_angle) >= CONFIG_DEGREE_MAX)
                        {
                            start_angle = start_angle + div_angle - CONFIG_DEGREE_MAX;
                        }
                        else
                        {
                            start_angle += div_angle;
                        }
                        start_time += div_time;
                    }
                }
                else
                {
                    u16_t start_angle = pack[0].depths[i].azimuth;
                    u16_t end_angle = pack[1].depths[0].azimuth;
                    u16_t div_angle;
                    if (end_angle >= start_angle)
                    {
                        div_angle = (end_angle - start_angle) / CONFIG_BLOCK_DEPTHS;
                    }
                    else
                    {
                        div_angle = (CONFIG_DEGREE_MAX - start_angle + end_angle) / CONFIG_BLOCK_DEPTHS;
                    }
                    for (u32_t j = 0; j < CONFIG_BLOCK_DEPTHS; j++)
                    {
                        udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].timestamp = start_time;
                        udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].angle = start_angle;
                        udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].distance = pack->depths[i].depth[j].distance0;
                        udprepack[urpmain % CONFIG_MAX_REPACK].dotcloud[tempdots % CONFIG_CIRCLE_DOTS].rssi = pack->depths[i].depth[j].rssi0;
                        udprepack[urpmain % CONFIG_MAX_REPACK].interval += div_time;
                        tempdots++;
                        if ((start_angle + div_angle) >= CONFIG_DEGREE_MAX)
                        {
                            start_angle = start_angle + div_angle - CONFIG_DEGREE_MAX;
                        }
                        else
                        {
                            start_angle += div_angle;
                        }
                        start_time += div_time;
                    }
                }
            }
            udprepack[urpmain % CONFIG_MAX_REPACK].maxdots = tempdots;
        }
    }
}
