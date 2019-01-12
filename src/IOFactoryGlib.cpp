/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.

    All source material (source code, images, sounds, etc.) have been provided to
    University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course without
    prior permission from the copyright holder.

    All sound files, sound fonts, midi files, and images that have been included 
    that were extracted from original Warcraft II by Blizzard Entertainment 
    were found freely available via internet sources and have been labeld as 
    abandonware. They have been included in this distribution for educational 
    purposes only and this copyright notice does not attempt to claim any 
    ownership of this material.
*/
#include "IOFactoryGlib.h"
#include <fcntl.h>
#include "FileDataSource.h"
#include "FileDataSink.h"

const uint32_t SIOEventType::EventIn = G_IO_IN;
const uint32_t SIOEventType::EventOut = G_IO_OUT;
const uint32_t SIOEventType::EventPriority = G_IO_PRI;
const uint32_t SIOEventType::EventError = G_IO_ERR;
const uint32_t SIOEventType::EventHangUp = G_IO_HUP;
const uint32_t SIOEventType::EventInvalid = G_IO_NVAL;
    
void SIOEventType::SetIn(){
    DValue |= EventIn;
}

void SIOEventType::SetOut(){
    DValue |= EventOut;
}

void SIOEventType::SetPriority(){
    DValue |= EventPriority;
}

void SIOEventType::SetError(){
    DValue |= EventError;
}

void SIOEventType::SetHangUp(){
    DValue |= EventHangUp;
}
  
void SIOEventType::SetInvalid(){
    DValue |= EventInvalid;
}


void SIOEventType::ClearIn(){
    DValue &= ~EventIn;
}

void SIOEventType::ClearOut(){
    DValue &= ~EventOut;
}

void SIOEventType::ClearPriority(){
    DValue &= ~EventPriority;
}

void SIOEventType::ClearError(){
    DValue &= ~EventError;
}

void SIOEventType::ClearHangUp(){
    DValue &= ~EventHangUp;
}
  
void SIOEventType::ClearInvalid(){
    DValue &= ~EventInvalid;
}


bool SIOEventType::IsIn() const{
    return DValue & EventIn;
}

bool SIOEventType::IsOut() const{
    return DValue & EventOut;
}

bool SIOEventType::IsPriority() const{
    return DValue & EventPriority;
}

bool SIOEventType::IsError() const{
    return DValue & EventError;
}

bool SIOEventType::IsHangUp() const{
    return DValue & EventHangUp;
}

bool SIOEventType::IsInvalid() const{
    return DValue & EventInvalid;
}
    

const uint32_t CIOFactory::SFileOptions::ReadOnly = O_RDONLY;
const uint32_t CIOFactory::SFileOptions::WriteOnly = O_WRONLY;
const uint32_t CIOFactory::SFileOptions::ReadWrite = O_RDWR;
const uint32_t CIOFactory::SFileOptions::Append = O_APPEND;
const uint32_t CIOFactory::SFileOptions::Truncate = O_TRUNC;
const uint32_t CIOFactory::SFileOptions::Create = O_CREAT;
const uint32_t CIOFactory::SFileOptions::Exclusive = O_EXCL;
            
void CIOFactory::SFileOptions::SetReadOnly(){
    DValue &= ~WriteOnly;
    DValue &= ~ReadWrite;
    DValue |= ReadOnly;
}

void CIOFactory::SFileOptions::SetWriteOnly(){
    DValue &= ~ReadOnly;
    DValue &= ~ReadWrite;
    DValue |= WriteOnly;
}

void CIOFactory::SFileOptions::SetReadWrite(){
    DValue &= ~ReadOnly;
    DValue &= ~WriteOnly;
    DValue |= ReadWrite;
}

void CIOFactory::SFileOptions::SetAppend(){
    DValue |= Append;
}

void CIOFactory::SFileOptions::SetTruncate(){
    DValue |= Truncate;
}

void CIOFactory::SFileOptions::SetCreate(){
    DValue |= Create;
}

void CIOFactory::SFileOptions::SetExclusive(){
    DValue |= Exclusive;
}


void CIOFactory::SFileOptions::ClearReadOnly(){

}

void CIOFactory::SFileOptions::ClearWriteOnly(){

}

void CIOFactory::SFileOptions::ClearReadWrite(){

}

void CIOFactory::SFileOptions::ClearAppend(){
    DValue &= ~Append;
}

void CIOFactory::SFileOptions::ClearTruncate(){
    DValue &= ~Truncate;
}

void CIOFactory::SFileOptions::ClearCreate(){
    DValue &= ~Create;
}

void CIOFactory::SFileOptions::ClearExclusive(){
    DValue &= ~Exclusive;
}

std::shared_ptr<CIOChannel> CIOFactory::NewFileIOChannel(const std::string &filename, const struct SFileOptions &options, std::shared_ptr<CDataSource> src, std::shared_ptr<CDataSink> sink){
    return std::make_shared<CIOChannelFileGlib>(open(filename.c_str(), options.DValue, 0600), src, sink);
}

std::shared_ptr<CIOChannel> CIOFactory::NewFileDescriptorIOChannel(int fd, std::shared_ptr<CDataSource> src, std::shared_ptr<CDataSink> sink){
    return std::make_shared<CIOChannelFileGlib>(fd, src, sink);
}

CIOChannelFileGlib::CIOChannelFileGlib(int fd, std::shared_ptr<CDataSource> src, std::shared_ptr<CDataSink> sink){
    int Flags = fcntl(fd, F_GETFL, 0);
    DChannel = g_io_channel_unix_new(fd);
    DFileHandle = fd;
    
    Flags &= CIOFactory::SFileOptions::ReadOnly | CIOFactory::SFileOptions::WriteOnly | CIOFactory::SFileOptions::ReadWrite;
    DDataSource = src;
    if(!DDataSource && ((CIOFactory::SFileOptions::ReadOnly == Flags)||(CIOFactory::SFileOptions::ReadWrite == Flags))){
        DDataSource = std::make_shared<CFileDataSource>("", DFileHandle); 
    }
    DDataSink = sink;
    if(!DDataSink && ((CIOFactory::SFileOptions::WriteOnly == Flags)||(CIOFactory::SFileOptions::ReadWrite == Flags))){
        DDataSink = std::make_shared<CFileDataSink>("", DFileHandle); 
    }    
}

CIOChannelFileGlib::~CIOChannelFileGlib(){
    GError *Error = NULL;
    DDataSource = nullptr;
    DDataSink = nullptr;
    
    g_io_channel_shutdown(DChannel, TRUE, &Error);
    if(DInTag){
        g_source_remove(DInTag);   
    }
    if(DOutTag){
        g_source_remove(DOutTag);   
    }
    if(DPriorityTag){
        g_source_remove(DPriorityTag);   
    }
    if(DErrorTag){
        g_source_remove(DErrorTag);   
    }
    if(DHangUpTag){
        g_source_remove(DHangUpTag);   
    }
    if(DInvalidTag){
        g_source_remove(DInvalidTag);   
    }
    g_io_channel_unref(DChannel);
}


gboolean CIOChannelFileGlib::InEventCallback(GIOChannel *source, GIOCondition condition, gpointer data){
    CIOChannelFileGlib *Channel = static_cast<CIOChannelFileGlib *>(data); 

    if(Channel->DInCallback){
        SIOEventType EventType;
        
        EventType.SetIn();
        if(Channel->DInCallback(Channel->shared_from_this(), EventType, Channel->DInCalldata)){
            return TRUE;
        }
    }
    Channel->DInTag = 0;
    return FALSE;
}

gboolean CIOChannelFileGlib::OutEventCallback(GIOChannel *source, GIOCondition condition, gpointer data){
    CIOChannelFileGlib *Channel = static_cast<CIOChannelFileGlib *>(data); 

    if(Channel->DOutCallback){
        SIOEventType EventType;
        
        EventType.SetOut();
        if(Channel->DOutCallback(Channel->shared_from_this(), EventType, Channel->DOutCalldata)){
            return TRUE;
        }
    }
    Channel->DOutTag = 0;
    return FALSE;
}

gboolean CIOChannelFileGlib::PriorityEventCallback(GIOChannel *source, GIOCondition condition, gpointer data){
    CIOChannelFileGlib *Channel = static_cast<CIOChannelFileGlib *>(data); 

    if(Channel->DPriorityCallback){
        SIOEventType EventType;
        
        EventType.SetPriority();
        if(Channel->DPriorityCallback(Channel->shared_from_this(), EventType, Channel->DPriorityCalldata)){
            return TRUE;
        }
    }
    Channel->DPriorityTag = 0;
    return FALSE;
}

gboolean CIOChannelFileGlib::ErrorEventCallback(GIOChannel *source, GIOCondition condition, gpointer data){
    CIOChannelFileGlib *Channel = static_cast<CIOChannelFileGlib *>(data); 

    if(Channel->DErrorCallback){
        SIOEventType EventType;
        
        EventType.SetError();
        if(Channel->DErrorCallback(Channel->shared_from_this(), EventType, Channel->DErrorCalldata)){
            return TRUE;
        }
    }
    Channel->DErrorTag = 0;
    return FALSE;
}

gboolean CIOChannelFileGlib::HangUpEventCallback(GIOChannel *source, GIOCondition condition, gpointer data){
    CIOChannelFileGlib *Channel = static_cast<CIOChannelFileGlib *>(data); 

    if(Channel->DHangUpCallback){
        SIOEventType EventType;
        
        EventType.SetHangUp();
        if(Channel->DHangUpCallback(Channel->shared_from_this(), EventType, Channel->DHangUpCalldata)){
            return TRUE;
        }
    }
    Channel->DHangUpTag = 0;
    return FALSE;
}

gboolean CIOChannelFileGlib::InvalidEventCallback(GIOChannel *source, GIOCondition condition, gpointer data){
    CIOChannelFileGlib *Channel = static_cast<CIOChannelFileGlib *>(data); 

    if(Channel->DInvalidCallback){
        SIOEventType EventType;
        
        EventType.SetInvalid();
        if(Channel->DInvalidCallback(Channel->shared_from_this(), EventType, Channel->DInvalidCalldata)){
            return TRUE;
        }
    }
    Channel->DInvalidTag = 0;
    return FALSE;
}


std::shared_ptr<CDataSource> CIOChannelFileGlib::CreateDataSouce(){
    return DDataSource;
}

std::shared_ptr<CDataSink> CIOChannelFileGlib::CreateDataSink(){
    return DDataSink;
}


void CIOChannelFileGlib::SetInEventCallback(TIOCalldata calldata, TIOCallback callback){
    DInCalldata = calldata;
    DInCallback = callback;
    if(callback){
        if(0 == DInTag){
            DInTag = g_io_add_watch(DChannel, G_IO_IN, InEventCallback, this);   
        }
    }
    else if(DInTag){
        g_source_remove(DInTag);
        DInTag = 0;
    }
}

void CIOChannelFileGlib::SetOutEventCallback(TIOCalldata calldata, TIOCallback callback){
    DOutCalldata = calldata;
    DOutCallback = callback;
    if(callback){
        if(0 == DOutTag){
            DOutTag = g_io_add_watch(DChannel, G_IO_OUT, OutEventCallback, this);   
        }
    }
    else if(DOutTag){
        g_source_remove(DOutTag);
        DOutTag = 0;
    }
}

void CIOChannelFileGlib::SetPriorityEventCallback(TIOCalldata calldata, TIOCallback callback){
    DPriorityCalldata = calldata;
    DPriorityCallback = callback;
    if(callback){
        if(0 == DPriorityTag){
            DPriorityTag = g_io_add_watch(DChannel, G_IO_PRI, PriorityEventCallback, this);   
        }
    }
    else if(DPriorityTag){
        g_source_remove(DPriorityTag);
        DPriorityTag = 0;
    }
}

void CIOChannelFileGlib::SetErrorEventCallback(TIOCalldata calldata, TIOCallback callback){
    DErrorCalldata = calldata;
    DErrorCallback = callback;
    if(callback){
        if(0 == DErrorTag){
            DErrorTag = g_io_add_watch(DChannel, G_IO_ERR, ErrorEventCallback, this);   
        }
    }
    else if(DErrorTag){
        g_source_remove(DErrorTag);
        DErrorTag = 0;
    }
}

void CIOChannelFileGlib::SetHangUpEventCallback(TIOCalldata calldata, TIOCallback callback){
    DHangUpCalldata = calldata;
    DHangUpCallback = callback;
    if(callback){
        if(0 == DHangUpTag){
            DHangUpTag = g_io_add_watch(DChannel, G_IO_HUP, HangUpEventCallback, this);   
        }
    }
    else if(DHangUpTag){
        g_source_remove(DHangUpTag);
        DHangUpTag = 0;
    }
}

void CIOChannelFileGlib::SetInvalidEventCallback(TIOCalldata calldata, TIOCallback callback){
    DInvalidCalldata = calldata;
    DInvalidCallback = callback;
    if(callback){
        if(0 == DInvalidTag){
            DInvalidTag = g_io_add_watch(DChannel, G_IO_NVAL, InvalidEventCallback, this);   
        }
    }
    else if(DInvalidTag){
        g_source_remove(DInvalidTag);
        DInvalidTag = 0;
    }
}


