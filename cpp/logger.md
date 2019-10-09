## 支持分片的FileWriter

```c++
class FileWriter {
public:
    FileWriter(std::string const& log_directory, std::string const& log_file_name, uint32_t log_file_roll_size_mb)
        : m_log_file_roll_size_bytes(log_file_roll_size_mb * 1024 * 1024)
        , m_name(log_directory + log_file_name)
    {
        roll_file();
    }

    void write(NanoLogLine& logline)
    {
		// 写入logline的内容
        auto pos = m_os->tellp();
        logline.stringify(*m_os);
        m_bytes_written += m_os->tellp() - pos;
		// 如果当前的文件大小已经超过了m_log_file_roll_size_bytes
		// 则创建一个新的文件写入接下来的日志
        if (m_bytes_written > m_log_file_roll_size_bytes) {
            roll_file();
        }
    }

private:
	// 关闭当前开启的文件 开启一个新的文件
    void roll_file()
    {
		//关闭可能开启的流
        if (m_os) {
            m_os->flush();//刷新缓冲区的数据至硬盘
            m_os->close();
        }
		
        m_bytes_written = 0; //已写的字节
        m_os.reset(new std::ofstream());  //开启一个流
        // TODO Optimize this part. Does it even matter ?
        std::string log_file_name = m_name;
		// 为日志文件添加后缀
		// 如 log_file_name.1.txt log_file_name.2.txt
        log_file_name.append(".");
        log_file_name.append(std::to_string(++m_file_number));
        log_file_name.append(".txt");
		// 以写模式开启文件 如果开启的文件非空 清空原来的内容
        m_os->open(log_file_name, std::ofstream::out | std::ofstream::trunc);
    }

private:
    uint32_t m_file_number = 0;
    std::streamoff m_bytes_written = 0;
    uint32_t const m_log_file_roll_size_bytes; //最大大小 单位为MB 因为 1024*1024字节= 1MB
    std::string const m_name; // 日志文件路径
    std::unique_ptr<std::ofstream> m_os;
};
```
