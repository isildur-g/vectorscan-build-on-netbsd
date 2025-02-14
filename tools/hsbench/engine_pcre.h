/*
 * Copyright (c) 2018, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ENGINEPCRE_H
#define ENGINEPCRE_H

#include "expressions.h"
#include "engine.h"

#include <pcre.h>

#include <memory>
#include <string>
#include <vector>

/** Infomation about the database compile */
struct CompilePCREStats {
    std::string sigs_name;
    std::string signatures;
    std::string db_info;
    size_t expressionCount = 0;
    size_t compiledSize = 0;
    size_t scratchSize = 0;
    long double compileSecs = 0;
    unsigned int peakMemorySize = 0;
};

/** Engine context which is allocated on a per-thread basis. */
class EnginePCREContext : public EngineContext{
public:
    explicit EnginePCREContext(int capture_cnt);
    virtual ~EnginePCREContext() override;

    int *ovec = nullptr;
};

struct PcreDB {
    bool highlander = false;
    bool utf8 = false;
    u32 id = 0;
    pcre *db = nullptr;
    pcre_extra *extra = nullptr;
};

/** PCRE Engine for scanning data. */
class EnginePCRE : public Engine {
public:
    explicit EnginePCRE(std::vector<std::unique_ptr<PcreDB>> dbs_in,
                        CompilePCREStats cs, int capture_cnt_in);
    virtual ~EnginePCRE() override;

    std::unique_ptr<EngineContext> makeContext() const override;

    void scan(const char *data, unsigned int len, unsigned int id,
              ResultEntry &result, EngineContext &ectx) const override;

    void scan_vectored(const char *const *data, const unsigned int *len,
                       unsigned int count, unsigned int streamId,
                       ResultEntry &result, EngineContext &ectx) const override;

    std::unique_ptr<EngineStream> streamOpen(EngineContext &ectx,
                                             unsigned id) const override;

    void streamClose(std::unique_ptr<EngineStream> stream,
                     ResultEntry &result) const override;

    void streamCompressExpand(EngineStream &stream,
                              std::vector<char> &temp) const override;

    void streamScan(EngineStream &stream, const char *data, unsigned int len,
                    unsigned int id, ResultEntry &result) const override;

    void printStats() const override;

    void printCsvStats() const override;

    void sqlStats(SqlDB &db) const override;

private:
    std::vector<std::unique_ptr<PcreDB>> dbs;

    CompilePCREStats compile_stats;

    int capture_cnt;
};

std::unique_ptr<EnginePCRE>
buildEnginePcre(const ExpressionMap &expressions, const std::string &name,
                const std::string &sigs_name);

#endif // ENGINEPCRE_H
