#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/Tensor.c"
#else

static void torch_Tensor_(c_readTensorStorageSizeStride)(lua_State *L, int index, int allowNone, int allowTensor, int allowStorage, int allowStride,
                                                         THStorage **storage_, long *storageOffset_, THLongStorage **size_, THLongStorage **stride_);

static void torch_Tensor_(c_readSizeStride)(lua_State *L, int index, int allowStride, THLongStorage **size_, THLongStorage **stride_);

static int torch_Tensor_(size)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  if(lua_isnumber(L,2))
  {
    int dim = luaL_checkint(L, 2)-1;
    luaL_argcheck(L, dim >= 0 && dim < tensor->nDimension, 2, "out of range");
    lua_pushnumber(L, tensor->size[dim]);
  }
  else
  {
    THLongStorage *storage = THLongStorage_newWithSize(tensor->nDimension);
    memmove(storage->data, tensor->size, sizeof(long)*tensor->nDimension);
    luaT_pushudata(L, storage, torch_LongStorage_id);
  }
  return 1;
}

static int torch_Tensor_(stride)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  if(lua_isnumber(L,2))
  {
    int dim = luaL_checkint(L, 2)-1;
    luaL_argcheck(L, dim >= 0 && dim < tensor->nDimension, 2, "out of range");
    lua_pushnumber(L, tensor->stride[dim]);
  }
  else
  {
    THLongStorage *storage = THLongStorage_newWithSize(tensor->nDimension);
    memmove(storage->data, tensor->stride, sizeof(long)*tensor->nDimension);
    luaT_pushudata(L, storage, torch_LongStorage_id);
  }
  return 1;
}

static int torch_Tensor_(nDimension)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  lua_pushnumber(L, tensor->nDimension);
  return 1;
}

static int torch_Tensor_(storage)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  THStorage_(retain)(tensor->storage);
  luaT_pushudata(L, tensor->storage, torch_Storage_id);
  return 1;
}

static int torch_Tensor_(storageOffset)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  lua_pushnumber(L, tensor->storageOffset+1);
  return 1;
}

static int torch_Tensor_(new)(lua_State *L)
{
  THTensor *tensor;
  THStorage *storage;
  long storageOffset;
  THLongStorage *size, *stride;

  torch_Tensor_(c_readTensorStorageSizeStride)(L, 1, 1, 1, 1, 1,
                                               &storage, &storageOffset, &size, &stride);

  tensor = THTensor_(newWithStorage)(storage, storageOffset, size, stride);

  THLongStorage_free(size);
  THLongStorage_free(stride);
  luaT_pushudata(L, tensor, torch_Tensor_id);
  return 1;
}

/* Resize */
static int torch_Tensor_(resizeAs)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  THTensor *src = luaT_checkudata(L, 2, torch_Tensor_id);
  THTensor_(resizeAs)(tensor, src);
  lua_settop(L, 1);
  return 1;
}

static int torch_Tensor_(resize)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  THLongStorage *size, *stride;

  torch_Tensor_(c_readSizeStride)(L, 2, 0, &size, &stride);

  THTensor_(resize)(tensor, size, stride);

  THLongStorage_free(size);
  THLongStorage_free(stride);

  lua_settop(L, 1);
  return 1;
}

static int torch_Tensor_(narrow)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  int dimension = luaL_checkint(L, 2)-1;
  long firstIndex = luaL_checklong(L, 3)-1;
  long size = luaL_checklong(L, 4);

/*  THArgCheck( (dimension >= 0) && (dimension < tensor->nDimension), 2, "out of range");
  THArgCheck( (firstIndex >= 0) && (firstIndex < tensor->size[dimension]), 3, "out of range");
  THArgCheck( (size > 0) && (firstIndex+size <= tensor->size[dimension]), 4, "out of range");
*/
  tensor = THTensor_(newWithTensor)(tensor);
  THTensor_(narrow)(tensor, dimension, firstIndex, size);
  luaT_pushudata(L, tensor, torch_Tensor_id);
  return 1;
}

static int torch_Tensor_(sub)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  long d0s = -1, d0e = -1, d1s = -1, d1e = -1, d2s = -1, d2e = -1, d3s = -1, d3e = -1;

  d0s = luaL_checklong(L, 2)-1;
  d0e = luaL_checklong(L, 3)-1;
  if(d0s < 0)
    d0s += tensor->size[0]+1;
  if(d0e < 0)
    d0e += tensor->size[0]+1;
  luaL_argcheck(L, tensor->nDimension > 0, 2, "invalid dimension");
  luaL_argcheck(L, d0s >= 0 && d0s < tensor->size[0], 2, "out of range");
  luaL_argcheck(L, d0e >= 0 && d0e < tensor->size[0], 3, "out of range");
  luaL_argcheck(L, d0e >= d0s, 3, "end smaller than beginning");

  if(!lua_isnone(L, 4))
  {
    d1s = luaL_checklong(L, 4)-1;
    d1e = luaL_checklong(L, 5)-1;
    if(d1s < 0)
      d1s += tensor->size[1]+1;
    if(d1e < 0)
      d1e += tensor->size[1]+1;
    luaL_argcheck(L, tensor->nDimension > 1, 4, "invalid dimension");
    luaL_argcheck(L, d1s >= 0 && d1s < tensor->size[1], 4, "out of range");
    luaL_argcheck(L, d1e >= 0 && d1e < tensor->size[1], 5, "out of range");    
    luaL_argcheck(L, d1e >= d1s, 5, "end smaller than beginning");

    if(!lua_isnone(L, 6))
    {
      d2s = luaL_checklong(L, 6)-1;
      d2e = luaL_checklong(L, 7)-1;
      if(d2s < 0)
        d2s += tensor->size[2]+1;
      if(d2e < 0)
        d2e += tensor->size[2]+1;
      luaL_argcheck(L, tensor->nDimension > 2, 6, "invalid dimension");
      luaL_argcheck(L, d2s >= 0 && d2s < tensor->size[2], 6, "out of range");
      luaL_argcheck(L, d2e >= 0 && d2e < tensor->size[2], 7, "out of range");    
      luaL_argcheck(L, d2e >= d2s, 7, "end smaller than beginning");

      if(!lua_isnone(L, 8))
      {
        d3s = luaL_checklong(L, 8)-1;
        d3e = luaL_checklong(L, 9)-1;
        if(d3s < 0)
          d3s += tensor->size[3]+1;
        if(d3e < 0)
          d3e += tensor->size[3]+1;
        luaL_argcheck(L, tensor->nDimension > 3, 8, "invalid dimension");
        luaL_argcheck(L, d3s >= 0 && d3s < tensor->size[3], 8, "out of range");
        luaL_argcheck(L, d3e >= 0 && d3e < tensor->size[3], 9, "out of range");    
        luaL_argcheck(L, d3e >= d3s, 9, "end smaller than beginning");
      }
    }
  }

  tensor = THTensor_(newWithTensor)(tensor);
  THTensor_(narrow)(tensor, 0, d0s, d0e-d0s+1);
  if(d1s >= 0)
    THTensor_(narrow)(tensor, 1, d1s, d1e-d1s+1);
  if(d2s >= 0)
    THTensor_(narrow)(tensor, 2, d2s, d2e-d2s+1);
  if(d3s >= 0)
    THTensor_(narrow)(tensor, 3, d3s, d3e-d3s+1);
  luaT_pushudata(L, tensor, torch_Tensor_id);
  return 1;
}

static int torch_Tensor_(select)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  int dimension = luaL_checkint(L, 2)-1;
  long sliceIndex = luaL_checklong(L, 3)-1;

/*   THArgCheck(src->nDimension > 1, 1, "cannot select on a vector");
  THArgCheck((dimension >= 0) && (dimension < src->nDimension), 2, "out of range");
  THArgCheck((sliceIndex >= 0) && (sliceIndex < src->size[dimension]), 3, "out of range");
*/

  tensor = THTensor_(newWithTensor)(tensor);
  THTensor_(select)(tensor, dimension, sliceIndex);
  luaT_pushudata(L, tensor, torch_Tensor_id);
  return 1;
}


static int torch_Tensor_(transpose)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  int dimension1 = luaL_checkint(L, 2)-1;
  int dimension2 = luaL_checkint(L, 3)-1;

/*
  THArgCheck( (dimension1 >= 0) && (dimension1 < src->nDimension), 2, "out of range");
  THArgCheck( (dimension2 >= 0) && (dimension2 < src->nDimension), 3, "out of range");
*/

  tensor = THTensor_(newWithTensor)(tensor);
  THTensor_(transpose)(tensor, dimension1, dimension2);
  luaT_pushudata(L, tensor, torch_Tensor_id);
  return 1;
}

static int torch_Tensor_(t)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);

  luaL_argcheck(L, tensor->nDimension == 2, 1, "Tensor must have 2 dimensions");

  tensor = THTensor_(newWithTensor)(tensor);
  THTensor_(transpose)(tensor, 0, 1);
  luaT_pushudata(L, tensor, torch_Tensor_id);
  return 1;
}

int torch_Tensor_(unfold)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  int dimension = luaL_checkint(L, 2)-1;
  long size = luaL_checklong(L, 3);
  long step = luaL_checklong(L, 4);

/*
  THArgCheck( (src->nDimension > 0), 1, "cannot unfold an empty tensor");
  THArgCheck(dimension < src->nDimension, 2, "out of range");
  THArgCheck(size <= src->size[dimension], 3, "out of range");
*/

  tensor = THTensor_(newWithTensor)(tensor);
  THTensor_(unfold)(tensor, dimension, size, step);
  luaT_pushudata(L, tensor, torch_Tensor_id);
  return 1;
}

/* is contiguous? [a bit like in TnXIterator] */
static int torch_Tensor_(isContiguous)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  lua_pushboolean(L, THTensor_(isContiguous)(tensor));
  return 1;
}

static int torch_Tensor_(nElement)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  lua_pushnumber(L, THTensor_(nElement)(tensor));
  return 1;
}

static int torch_Tensor_(copy)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  void *src;
  if( (src = luaT_toudata(L, 2, torch_Tensor_id)) )
    THTensor_(copy)(tensor, src);
  else if( (src = luaT_toudata(L, 2, torch_ByteTensor_id)) )
    THTensor_(copyByte)(tensor, src);
  else if( (src = luaT_toudata(L, 2, torch_CharTensor_id)) )
    THTensor_(copyChar)(tensor, src);
  else if( (src = luaT_toudata(L, 2, torch_ShortTensor_id)) )
    THTensor_(copyShort)(tensor, src);
  else if( (src = luaT_toudata(L, 2, torch_IntTensor_id)) )
    THTensor_(copyInt)(tensor, src);
  else if( (src = luaT_toudata(L, 2, torch_LongTensor_id)) )
    THTensor_(copyLong)(tensor, src);
  else if( (src = luaT_toudata(L, 2, torch_FloatTensor_id)) )
    THTensor_(copyFloat)(tensor, src);
  else if( (src = luaT_toudata(L, 2, torch_Tensor_id)) )
    THTensor_(copyDouble)(tensor, src);
  else
    luaL_typerror(L, 2, "torch.*Tensor");
  lua_settop(L, 1);
  return 1;
}

static int torch_Tensor_(__newindex__)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  THLongStorage *idx = NULL;

  if(lua_isnumber(L, 2))
  {
    long index = luaL_checklong(L,2)-1;
    real value = (real)luaL_checknumber(L,3);
    luaL_argcheck(L, tensor->nDimension == 1, 1, "must be a one dimensional tensor");
    luaL_argcheck(L, index >= 0 && index < tensor->size[0], 2, "out of range");
    (tensor->storage->data+tensor->storageOffset)[index*tensor->stride[0]] = value;
    lua_pushboolean(L, 1);
  }
  else if((idx = luaT_toudata(L, 2, torch_LongStorage_id)))
  {
    real *data = THTensor_(data)(tensor);
    real value = (real)luaL_checknumber(L,3);
    int dim;

    luaL_argcheck(L, 2, idx->size == tensor->nDimension, "invalid size");
    
    for(dim = 0; dim < idx->size; dim++)
    {
      long z = idx->data[dim]-1;
      luaL_argcheck(L, 2, (z >= 0) && (z < tensor->size[dim]), "index out of bound");
      data += z*tensor->stride[dim];
    }

    *data = value;
    lua_pushboolean(L, 1);
  }
  else
    lua_pushboolean(L, 0);

  return 1;
}

static int torch_Tensor_(__index__)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  THLongStorage *idx = NULL;

  if(lua_isnumber(L, 2))
  {
    long index = luaL_checklong(L,2)-1;
    
    luaL_argcheck(L, tensor->nDimension > 0, 1, "empty tensor");
    luaL_argcheck(L, index >= 0 && index < tensor->size[0], 2, "out of range");

    if(tensor->nDimension == 1)
    {
      lua_pushnumber(L, (tensor->storage->data+tensor->storageOffset)[index*tensor->stride[0]]);
    }
    else
    {
      tensor = THTensor_(newWithTensor)(tensor);
      THTensor_(select)(tensor, 0, index);
      luaT_pushudata(L, tensor, torch_Tensor_id);
    }
    lua_pushboolean(L, 1);
    return 2;
  }
  else if((idx = luaT_toudata(L, 2, torch_LongStorage_id)))
  {
    real *data = THTensor_(data)(tensor);
    int dim;

    luaL_argcheck(L, 2, idx->size == tensor->nDimension, "invalid size");
    
    for(dim = 0; dim < idx->size; dim++)
    {
      long z = idx->data[dim]-1;
      luaL_argcheck(L, 2, (z >= 0) && (z < tensor->size[dim]), "index out of bound");
      data += z*tensor->stride[dim];
    }
    lua_pushnumber(L, *data);
    lua_pushboolean(L, 1);
    return 2;
  }
  else
  {
    lua_pushboolean(L, 0);
    return 1;
  }
}

static int torch_Tensor_(free)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  THTensor_(free)(tensor);
  return 0;
}

/* helpful functions */
static void torch_Tensor_(c_readSizeStride)(lua_State *L, int index, int allowStride, THLongStorage **size_, THLongStorage **stride_)
{
  THLongStorage *size = NULL;
  THLongStorage *stride = NULL;
  
  if( (size = luaT_toudata(L, index, torch_LongStorage_id)) )
  {
    if(!lua_isnoneornil(L, index+1))
    {
      if( (stride = luaT_toudata(L, index+1, torch_LongStorage_id)) )
        luaL_argcheck(L, stride->size == size->size, index+1, "provided stride and size are inconsistent");
      else
        luaL_argcheck(L, 0, index+1, "torch.LongStorage expected");
    }
    THLongStorage_retain(size);
    if(stride)
      THLongStorage_retain(stride);
  }
  else
  {
    int i;

    size = THLongStorage_newWithSize(4);
    stride = THLongStorage_newWithSize(4);
    THLongStorage_fill(size, -1);
    THLongStorage_fill(stride, -1);

    if(allowStride)
    {
      for(i = 0; i < 4; i++)
      {
        if(lua_isnone(L, index+2*i))
          break;
        size->data[i] = luaL_checklong(L, index+2*i);
        
        if(lua_isnone(L, index+2*i+1))
          break;
        stride->data[i] = luaL_checklong(L, index+2*i+1);
      }
    }
    else
    {
      for(i = 0; i < 4; i++)
      {
        if(lua_isnone(L, index+i))
          break;
        size->data[i] = luaL_checklong(L, index+i);
      }
    }
  }

  *size_ = size;
  *stride_ = stride;
}

static void torch_Tensor_(c_readTensorStorageSizeStride)(lua_State *L, int index, int allowNone, int allowTensor, int allowStorage, int allowStride,
                                                         THStorage **storage_, long *storageOffset_, THLongStorage **size_, THLongStorage **stride_)
{
  static char errMsg[64];
  THTensor *src = NULL;
  THStorage *storage = NULL;

  int arg1Type = lua_type(L, index);

  if( allowNone && (arg1Type == LUA_TNONE) )
  {
    *storage_ = NULL;
    *storageOffset_ = 0;
    *size_ = NULL;
    *stride_ = NULL;
    return;
  }
  else if( allowTensor && (arg1Type == LUA_TUSERDATA) && (src = luaT_toudata(L, index, torch_Tensor_id)) )
  {
    *storage_ = src->storage;
    *storageOffset_ = src->storageOffset;
    *size_ = THTensor_(newSizeOf)(src);
    *stride_ = THTensor_(newStrideOf)(src);
    return;
  }
  else if( allowStorage && (arg1Type == LUA_TUSERDATA) && (storage = luaT_toudata(L, index, torch_Storage_id)) )
  {
    *storage_ = storage;
    if(lua_isnone(L, index+1))
    {
      *storageOffset_ = 0;
      *size_ = THLongStorage_newWithSize1(storage->size);
      *stride_ = THLongStorage_newWithSize1(1);
    }
    else
    {
      *storageOffset_ = luaL_checklong(L, index+1)-1;
      torch_Tensor_(c_readSizeStride)(L, index+2, allowStride, size_, stride_);
    }
    return;
  }
  else if( (arg1Type == LUA_TNUMBER) || (luaT_toudata(L, index, torch_LongStorage_id)) )
  {
    *storage_ = NULL;
    *storageOffset_ = 0;
    torch_Tensor_(c_readSizeStride)(L, index, 0, size_, stride_);

    return;
  }

  *storage_ = NULL;
  *storageOffset_ = 0;

  sprintf(errMsg, "expecting number%s%s", (allowTensor ? " or Tensor" : ""), (allowStorage ? " or Storage" : ""));
  luaL_argcheck(L, 0, index, errMsg);
}

static int torch_Tensor_(apply)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_settop(L, 2);

  TH_TENSOR_APPLY(real, tensor,
                  lua_pushvalue(L, 2);
                  lua_pushnumber(L, *tensor_data);
                  lua_call(L, 1, 1);
                  if(lua_isnumber(L, 3))
                  {
                    *tensor_data = (real)lua_tonumber(L, 3);
                    lua_pop(L, 1);
                  }
                  else if(lua_isnil(L, 3))
                    lua_pop(L, 1);
                  else
                    luaL_error(L, "given function should return a number or nil"););

  lua_settop(L, 1);
  return 1;
}

static int torch_Tensor_(map)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  THTensor *src = luaT_checkudata(L, 2, torch_Tensor_id);
  luaL_checktype(L, 3, LUA_TFUNCTION);
  lua_settop(L, 3);

  TH_TENSOR_APPLY2(real, tensor, real, src,
                  lua_pushvalue(L, 3);
                  lua_pushnumber(L, *tensor_data);
                  lua_pushnumber(L, *src_data);
                  lua_call(L, 2, 1);
                  if(lua_isnumber(L, 4))
                  {
                    *tensor_data = (real)lua_tonumber(L, 4);
                    lua_pop(L, 1);
                  }
                  else if(lua_isnil(L, 4))
                    lua_pop(L, 1);
                  else
                    luaL_error(L, "given function should return a number or nil"););

  lua_settop(L, 1);
  return 1;
}

static int torch_Tensor_(map2)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  THTensor *src1 = luaT_checkudata(L, 2, torch_Tensor_id);
  THTensor *src2 = luaT_checkudata(L, 3, torch_Tensor_id);
  luaL_checktype(L, 4, LUA_TFUNCTION);
  lua_settop(L, 4);

  TH_TENSOR_APPLY3(real, tensor, real, src1, real, src2,
                  lua_pushvalue(L, 4);
                  lua_pushnumber(L, *tensor_data);
                  lua_pushnumber(L, *src1_data);
                  lua_pushnumber(L, *src2_data);
                  lua_call(L, 3, 1);
                  if(lua_isnumber(L, 5))
                  {
                    *tensor_data = (real)lua_tonumber(L, 5);
                    lua_pop(L, 1);
                  }
                  else if(lua_isnil(L, 5))
                    lua_pop(L, 1);
                  else
                    luaL_error(L, "given function should return a number or nothing"););

  lua_settop(L, 1);
  return 1;
}

static int torch_Tensor_(factory)(lua_State *L)
{
  THTensor *tensor = THTensor_(new)();
  luaT_pushudata(L, tensor, torch_Tensor_id);
  return 1;
}

static int torch_Tensor_(write)(lua_State *L)
{  
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  long storageOffset = tensor->storageOffset+1; /* to respect Lua convention */

  lua_pushvalue(L, 2);
  torch_File_writeInt(L, &tensor->nDimension, 1);
  torch_File_writeLong(L, tensor->size, tensor->nDimension);
  torch_File_writeLong(L, tensor->stride, tensor->nDimension);
  torch_File_writeLong(L, &storageOffset, 1);
  THStorage_(retain)(tensor->storage);
  luaT_pushudata(L, tensor->storage, torch_Storage_id);
  torch_File_writeObject(L);
  return 0;
}

static int torch_Tensor_(read)(lua_State *L)
{
  THTensor *tensor = luaT_checkudata(L, 1, torch_Tensor_id);
  int version = luaL_checkint(L, 3);
  long storageOffset;
  
  lua_pushvalue(L, 2);
  torch_File_readInt(L, &tensor->nDimension, 1);
  tensor->size = THAlloc(sizeof(long)*tensor->nDimension);
  tensor->stride = THAlloc(sizeof(long)*tensor->nDimension);
  if(version > 0)
  {
    torch_File_readLong(L, tensor->size, tensor->nDimension);
    torch_File_readLong(L, tensor->stride, tensor->nDimension);
    torch_File_readLong(L, &storageOffset, 1);
  }
  else
  {
    int *buffer_ = THAlloc(sizeof(int)*tensor->nDimension);
    int storageOffset_;
    int i;

    torch_File_readInt(L, buffer_, tensor->nDimension);
    for(i = 0; i < tensor->nDimension; i++)
      tensor->size[i] = buffer_[i];

    torch_File_readInt(L, buffer_, tensor->nDimension);
    for(i = 0; i < tensor->nDimension; i++)
      tensor->stride[i] = buffer_[i];

    torch_File_readInt(L, &storageOffset_, 1);
    storageOffset = storageOffset_;
    THFree(buffer_);
  }
  tensor->storageOffset = storageOffset-1; /* to respect Lua convention */
  torch_File_readObject(L);  
  tensor->storage = luaT_toudata(L, -1, torch_Storage_id);
  THStorage_(retain)(tensor->storage);
  lua_pop(L, 1);
  return 0;
}

static const struct luaL_Reg torch_Tensor_(_) [] = {
  {"size", torch_Tensor_(size)},
  {"__len__", torch_Tensor_(size)},
  {"stride", torch_Tensor_(stride)},
  {"dim", torch_Tensor_(nDimension)},
  {"nDimension", torch_Tensor_(nDimension)},
  {"storage", torch_Tensor_(storage)},
  {"storageOffset", torch_Tensor_(storageOffset)},
  {"resizeAs", torch_Tensor_(resizeAs)},
  {"resize", torch_Tensor_(resize)},
  {"narrow", torch_Tensor_(narrow)},
  {"sub", torch_Tensor_(sub)},
  {"select", torch_Tensor_(select)},
  {"transpose", torch_Tensor_(transpose)},
  {"t", torch_Tensor_(t)},
  {"unfold", torch_Tensor_(unfold)},
  {"isContiguous", torch_Tensor_(isContiguous)},
  {"nElement", torch_Tensor_(nElement)},
  {"copy", torch_Tensor_(copy)},
  {"apply", torch_Tensor_(apply)},
  {"map", torch_Tensor_(map)},
  {"map2", torch_Tensor_(map2)},
  {"read", torch_Tensor_(read)},
  {"write", torch_Tensor_(write)},
  {"__index__", torch_Tensor_(__index__)},
  {"__newindex__", torch_Tensor_(__newindex__)},
  {NULL, NULL}
};

void torch_Tensor_(init)(lua_State *L)
{
  torch_ByteStorage_id = luaT_checktypename2id(L, "torch.ByteStorage");
  torch_CharStorage_id = luaT_checktypename2id(L, "torch.CharStorage");
  torch_ShortStorage_id = luaT_checktypename2id(L, "torch.ShortStorage");
  torch_IntStorage_id = luaT_checktypename2id(L, "torch.IntStorage");
  torch_LongStorage_id = luaT_checktypename2id(L, "torch.LongStorage");
  torch_FloatStorage_id = luaT_checktypename2id(L, "torch.FloatStorage");
  torch_DoubleStorage_id = luaT_checktypename2id(L, "torch.DoubleStorage");

  torch_Tensor_id = luaT_newmetatable(L, STRING_torchTensor, NULL,
                                 torch_Tensor_(new), torch_Tensor_(free), torch_Tensor_(factory));
  luaL_register(L, NULL, torch_Tensor_(_));
  lua_pop(L, 1);
}

#endif