/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtNetwork module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published by
 ** IBM.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL along with this file.
 ** See the LICENSE file and the cpl1.0.txt file included with the source
 ** distribution for more information. If you did not receive a copy of the
 ** license, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/

#include <qxtdatastreamsignalserializer.h>
#include <QIODevice>
#include <QDataStream>
#include <qtglobal.h>
#if QT_VERSION < 0x040300
#    include <string.h>
#endif

QByteArray QxtDataStreamSignalSerializer::serialize(QString fn, QVariant p1, QVariant p2, QVariant p3, QVariant p4, QVariant p5, QVariant p6, QVariant p7, QVariant p8, QVariant p9) const
{
    QByteArray rv;
    QDataStream str(&rv, QIODevice::WriteOnly);
    str << fn;
    unsigned char ct = 9;
    if (p1.isNull()) ct = 0;
    else if (p2.isNull()) ct = 1;
    else if (p3.isNull()) ct = 2;
    else if (p4.isNull()) ct = 3;
    else if (p5.isNull()) ct = 4;
    else if (p6.isNull()) ct = 5;
    else if (p7.isNull()) ct = 6;
    else if (p8.isNull()) ct = 7;
    else if (p9.isNull()) ct = 8;
    str << ct;
    if (ct-- >0 ) str << p1;
    if (ct-- >0) str << p2;
    if (ct-- >0) str << p3;
    if (ct-- >0) str << p4;
    if (ct-- >0) str << p5;
    if (ct-- >0) str << p6;
    if (ct-- >0) str << p7;
    if (ct-- >0) str << p8;
    if (ct-- >0) str << p9;
	uchar sizeData[4];
#if QT_VERSION >= 0x040300
    qToLittleEndian(rv.size(), sizeData);
#elif Q_BYTE_ORDER == Q_BIG_ENDIAN
    quint32 sz = rv.size();
    memcpy(sizeData, reinterpret_cast<char*>(&sz), 4);
#else
    quint32 sz = rv.size();
    memcpy(sizeData, reinterpret_cast<char*>(&sz), 4);
#endif
    return rv;
}

DeserializedData QxtDataStreamSignalSerializer::deserialize(QByteArray& data)
{
    QByteArray cmd;
    int pos = data.indexOf('\n');
    
    cmd = data.left(pos);
    data = data.mid(pos+1);
    if (cmd.length()==0) return NoOp();

    cmd.replace(QByteArray("\\n"), QByteArray("\n"));
    cmd.replace(QByteArray("\\\\"), QByteArray("\\"));
    QDataStream str(cmd);

    QString signal;
    unsigned char argCount;
    QList<QVariant> v;
    QVariant t;
    str >> signal >> argCount;

    if(str.status() == QDataStream::ReadCorruptData) return ProtocolError();

    for (int i=0; i<argCount; i++)
    {
        str >> t;
        v << t;
    }
    return qMakePair(signal, v);
}

bool QxtDataStreamSignalSerializer::canDeserialize(const QByteArray& buffer) const {
    return buffer.indexOf('\n') != -1;
}