//
// Created by zhihu on 2022/1/26.
//


#include "limits.h"

#include "redismodule.h"

#include "miaosha.h"

static RedisModuleType *StringType;

int DecrByNoLess_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);

    if (argc != 4) {
        return RedisModule_WrongArity(ctx);
    }

    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
    if (REDISMODULE_KEYTYPE_EMPTY != type && RedisModule_ModuleTypeGetType(key) != StringType) {
        RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
        return REDISMODULE_ERR;
    }

    long long decr;
    if (RedisModule_StringToLongLong(argv[2], &decr) != REDISMODULE_OK) {
        RedisModule_ReplyWithError(ctx, MIAOSHA_ERRORMSG_PARAMS_NO_INT);
        return REDISMODULE_ERR;
    }

    long long bound;
    if (RedisModule_StringToLongLong(argv[3], &bound) != REDISMODULE_OK) {
        RedisModule_ReplyWithError(ctx, MIAOSHA_ERRORMSG_PARAMS_NO_INT);
        return REDISMODULE_ERR;
    }
    long long value;
    if (REDISMODULE_KEYTYPE_EMPTY == type) {
        value = 0;
    } else {
        RedisModuleCallReply *replay = RedisModule_Call(ctx, "GET", "s", argv[1]);
        if (RedisModule_StringToLongLong(RedisModule_CreateStringFromCallReply(replay), &value) == REDISMODULE_ERR) {
            RedisModule_ReplyWithError(ctx, MIAOSHA_ERRORMSG_VALUE_NO_INT);
            return REDISMODULE_ERR;
        }
        if ((decr < 0 && value < 0 && decr < (LLONG_MIN - value)) ||
            (decr > 0 && value > 0 && decr > (LLONG_MAX - value)) ||
            value + decr < bound) {
            RedisModule_ReplyWithError(ctx, MIAOSHA_ERRORMSG_OVERFLOW);
            return REDISMODULE_ERR;
        }
    }
    value -= decr;

    RedisModule_StringSet(key, RedisModule_CreateStringFromLongLong(ctx, value));
    RedisModule_Replicate(ctx, "SET", "sl", argv[1], value);
    RedisModule_ReplyWithLongLong(ctx, value);
    return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if (RedisModule_Init(ctx, "miaosha", 1, REDISMODULE_APIVER_1)
        == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "incrbynoless",
                                  DecrByNoLess_RedisCommand, "write deny-oom",
                                  0, 0, 0) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    return REDISMODULE_OK;
}