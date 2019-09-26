#ifndef _OSGLLOGER_H_
#define _OSGLLOGER_H_

#include "dc_global.h"

#include <iostream>
#include <streambuf>
#include <string>
#include <QtCore/QObject>
#include <QtCore/QString>

class DC_EXPORT OsgLogger : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT

public:

    OsgLogger(std::ostream &stream) :
        m_stream(stream)
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }

    ~OsgLogger()
    {
        // 输出剩余的信息
        if ( !m_string.empty() )
            emit message( QString( m_string.c_str() ) );

        m_stream.rdbuf(m_old_buf);
    }

signals:

    void message(const QString &);

protected:

    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            emit message( QString( m_string.c_str() ) );
            m_string.erase( m_string.begin(), m_string.end() );
        }
        else
            m_string += v;

        return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);

        size_t pos = 0;
        while (pos != std::string::npos)
        {
            pos = m_string.find('\n');
            if (pos != std::string::npos)
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);
                emit message( QString( tmp.c_str() ) );
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }

        return n;
    }

private:

    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
};

#endif // _OSGLLOGER_H_
