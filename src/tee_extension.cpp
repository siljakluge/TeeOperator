#define DUCKDB_EXTENSION_MAIN
// ./build/release/duckdb  
/*
CREATE TABLE my_table (id INTEGER, name VARCHAR);
INSERT INTO my_table VALUES (1, 'Alice'), (2, 'Bob'), (3, 'Charlie');
SELECT * FROM tee((TABLE my_table));
*/
#include "tee_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>
#include "duckdb/common/printer.hpp"
#include "duckdb/function/table_function.hpp"
#include "iostream"

namespace duckdb {
        

    static void TeeTableFunction(ClientContext &context, TableFunctionInput &data, DataChunk &input, DataChunk &output) {
        input.Print();
        input.Reference(output);
    }

    static unique_ptr<FunctionData> TeeTableFunctionBind(ClientContext &context, TableFunctionBindInput &input,
                                                           vector<LogicalType> &return_types, vector<string> &names) {
        for(idx_t i = 0; i < input.input_table_types.size(); i++) {
            std::cout << "TeeTableFunctionBind: " << i << ": " << input.input_table_types[i].ToString() << std::endl; 
            std::cout << "TeeTableFunctionBind: " << i << ": " << input.input_table_names[i] << std::endl;
            return_types.push_back(input.input_table_types[i]);
            names.push_back(input.input_table_names[i]);
        }
        return nullptr;
    }

    
    
    static void LoadInternal(DatabaseInstance &instance) {
        vector<LogicalType> arg_types = {LogicalType::TABLE};
        string name = "tee";
        TableFunction tee_function(name,  {LogicalType::TABLE}, TeeTableFunction, TeeTableFunctionBind, nullptr);
        ExtensionUtil::RegisterFunction(instance, tee_function);
    }

void TeeExtension::Load(DuckDB &db) {
    LoadInternal(*db.instance);
}

std::string TeeExtension::Name() {
    return "tee";
}

std::string TeeExtension::Version() const {
#ifdef EXT_VERSION_TEE
    return EXT_VERSION_TEE;
#else
    return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void tee_init(duckdb::DatabaseInstance &db) {
    duckdb::DuckDB db_wrapper(db);
    db_wrapper.LoadExtension<duckdb::TeeExtension>();
}

DUCKDB_EXTENSION_API const char *tee_version() {
    return duckdb::DuckDB::LibraryVersion();
}

}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif