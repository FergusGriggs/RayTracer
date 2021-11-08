#pragma once

struct Header
{
	unsigned int m_checkCode;
	size_t       m_size;

	Header* m_prev;
	Header* m_next;
};

struct Footer
{
	unsigned int m_checkCode;
};