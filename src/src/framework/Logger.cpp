/*
 * Copyright (c) 2015 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Intel Corporation nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Logger.h"

cli::framework::LogMessage::LogMessage(Priority priority, std::string message)
: m_priority(priority), m_message(message), m_fileName(""), m_lineNumber(0)
{
}

cli::framework::LogMessage::LogMessage()
: m_priority(PRIORITY_DEBUG), m_message(""), m_fileName(""), m_lineNumber(0)
{}

std::string cli::framework::LogMessage::getMessage() const
{
	return m_message;
}

void cli::framework::LogMessage::setMessage(const std::string& message)
{
	m_message = message;
}

cli::framework::LogMessage::Priority cli::framework::LogMessage::getPriority() const
{
	return m_priority;
}

void cli::framework::LogMessage::setPriority(const Priority& priority)
{
	m_priority = priority;
}

const std::string& cli::framework::LogMessage::getFileName() const
{
	return m_fileName;
}

void cli::framework::LogMessage::setFileName(const std::string& fileName)
{
	m_fileName = fileName;
}

int cli::framework::LogMessage::getLineNumber() const
{
	return m_lineNumber;
}

void cli::framework::LogMessage::setLineNumber(const int& lineNumber)
{
	m_lineNumber = lineNumber;
}

/*
 * StreamChannel: How logs are written
 */
void cli::framework::StreamChannel::write(const LogMessage& message)
{
	(*m_pStream) << message.getPriority() << ": " << message.getMessage();
}


#ifdef __ESX__

/*
 * SysLogChannel: How logs are written
 * TODO: This has not been tested yet. Need to be able to build completely on ESX and run CLI
 */
void cli::framework::EsxLogChannel::write(const LogMessage& message)
{
	setlogmask (LOG_UPTO (LOG_ERR)); // only log errors and above

	// only write if priority is lower than configured
	// openlog ("crystalridge", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	if (message.getPriority() == LogMessage::PRIORITY_ERROR)
	{
		syslog (LOG_ERR, message.getMessage().c_str());
	}
	else
	{
		std::cout << message.getMessage();
	}

	closelog ();
}

#endif

void cli::framework::Logger::flush()
{
	LogMessage message(m_currentMessagePriority, _buffer.str());
	log(message);
	_buffer.clear();
	_buffer.str("");
}

void cli::framework::Logger::setChannel(LogChannelBase* pChannel)
{
	m_pChannel = pChannel;
}

cli::framework::LogChannelBase* cli::framework::Logger::getChannel()
{
	return m_pChannel;
}

void cli::framework::Logger::log(const LogMessage& message)
{
	if (m_pChannel && (message.getPriority() <= m_level))
	{
		m_pChannel->write(message);
	}
}

void cli::framework::Logger::log(const LogMessage::Priority& priority,
		const std::string& message)
{
	LogMessage logMessage(priority, message);
	log(logMessage);
}

void cli::framework::Logger::trace(const std::string& message)
{
	log(LogMessage::PRIORITY_DEBUG, message);
}

void cli::framework::Logger::trace(const std::string& message,
		const std::string& fileName, const int& lineNumber)
{
	LogMessage logMessage(LogMessage::PRIORITY_DEBUG, message);
	logMessage.setFileName(fileName);
	logMessage.setLineNumber(lineNumber);
	log(logMessage);
}

cli::framework::LogMessage::Priority cli::framework::Logger::getLevel() const
{
	return m_level;
}

void cli::framework::Logger::setLevel(const LogMessage::Priority& level)
{
	m_level = level;
}

