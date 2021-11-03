import os
from enum import Enum
import sys
import re

# GLOBAL VARIABLES REGION
abort_on_error = 0
argCount = -1
directoryOnly = 1
print_results = 0
output_file_name = None
protobuf_version = "proto3"
parsing_structs = []
packages = []


class TypeModifier(Enum):
    POINTER = 1
    ARRAY = 2


class Field:
    def __init__(self, value, proto_type, identified=1, type_modifier=None, array_size=None):
        self.value = value
        self.proto_type = proto_type
        self.identified = identified
        self.type_modifier = type_modifier
        self.array_size = array_size

    def print(self):
        print_text = "name = " + self.value + ", proto = " + self.proto_type
        if self.type_modifier is not None:
            print_text += ", modifier = " + str(self.type_modifier)
        if self.array_size is not None:
            print_text += ", size = " + str(self.array_size)
        if self.identified == 0:
            print_text += ", NOT IDENTIFIED"
        print(print_text)


class Struct:
    def __init__(self):
        self.fields = []
        self.name = ""
        self.is_enum = 0

    def append_field(self, field):
        self.fields.append(field)

    def print(self):
        for field in self.fields:
            field.print()


token_to_type_dictionary = [
    {
        'tokens': "double",
        'proto_type': "double"
    },
    {
        'tokens': "int",
        'proto_type': "int32"
    },
    {
        'tokens': "unsigned int",
        'proto_type': "uint32"
    },
    {
        'tokens': "float",
        'proto_type': "float"
    },
    {
        'tokens': "long",
        'proto_type': "int64"
    },
    {
        'tokens': "unsigned long",
        'proto_type': "uint64"
    },
    {
        'tokens': "long long",
        'proto_type': "int64"
    },
    {
        'tokens': "long double",
        'proto_type': "double"
    },
    {
        'tokens': "unsigned long long",
        'proto_type': "uint64"
    },
    {
        'tokens': "char",
        'proto_type': "string"
    },
    {
        'tokens': "unsigned char",
        'proto_type': "string"
    }
]

# adding reserved tokens, taking them from token_to_type_dictionary
reserved_tokens = set()
proto_tokens = set()

for token in token_to_type_dictionary:
    for tt in token['tokens'].split(" "):
        reserved_tokens.add(tt)
    proto_tokens.add(token['proto_type'])

reserved_tokens.add("*")
reserved_tokens.add("const")


# end


def file_path_and_file_name(file_name):
    if file_name.__contains__("/"):
        sep = "/"
    else:
        sep = "\\"
    file_name_with_ext = file_name.split(sep)
    if len(file_name_with_ext) < 1:
        path = ""
    else:
        path = os.sep.join(file_name_with_ext[:len(file_name_with_ext) - 1])
    return {
        'path': path,
        'file_name': file_name_with_ext[len(file_name_with_ext) - 1].split(".")[0]
    }


def print_error_and_exit(line_number, message):
    if line_number != -1:
        print("Line " + str(line_number) + ": " + message)
    else:
        print(message)
    if abort_on_error == 1:
        sys.exit()


def split_line_into_parts(line):
    p = re.compile("\\s")
    m_parts = p.split(line)
    parts = []
    for part in m_parts:
        p = part
        if p == "":
            continue
        if p != "*" and p.__contains__("*"):
            index = p.index("*")
            p = part[:index]
            if p == "":
                parts.append("*")
            else:
                parts.append(p.strip())
                parts.append("*")
            p = part[index + 1:]
            if p != "":
                parts.append(p)
            continue
        parts.append(p.strip())
    return parts


def process_variable(line_parts, line_number, struct, start_index, proto_type, type_modifier=None):
    variable_name = line_parts[start_index]
    if variable_name.__contains__(":"):
        variable_name = variable_name[:variable_name.index(":")]
    start_found = 0
    try:
        index_of_array_start = variable_name.index("[")
        start_found = 1
        index_of_array_end = variable_name.index("]")
        size = int(variable_name[index_of_array_start + 1:index_of_array_end])
        variable_name = variable_name[0:index_of_array_start].strip()
        struct.append_field(Field(variable_name, proto_type, 1, TypeModifier.ARRAY, size))
        return
    except ValueError:
        if start_found == 1:
            print_error_and_exit(line_number, "Found '[' but don't found ']' seems that syntax is wrong.")
        else:
            if struct.is_enum == 0:
                struct.append_field(Field(variable_name, proto_type, 1, type_modifier))
            else:
                struct.append_field(Field("", proto_type, 1, type_modifier))
        # it is not an array
        pass


def handle_simple_types(line_parts, line_number, struct):
    if struct is None:
        print_error_and_exit(line_number, "Illegal state exception, token received while struct is None")
        return 0
    # going from the end of the line to collect all reserved words to identify type
    tokens = []
    variable_index = len(line_parts)
    for part in reversed(line_parts):
        if is_reserved_token(part) == 1:
            tokens.insert(0, part)
            continue
        variable_index -= 1
    if tokens_not_parsed(tokens) == 1:
        return 0
    m_token = find_in_dictionary(tokens)
    type_modifier = None
    if tokens.__contains__("*"):
        type_modifier = TypeModifier.POINTER
    try:
        process_variable(line_parts, line_number, struct, variable_index, m_token['proto_type'], type_modifier)
    except TypeError:
        struct.append_field(Field("", " ".join(line_parts)))
        pass
    return 1


def tokens_not_parsed(tokens):
    if not tokens or tokens == ["*"] or tokens == ["const"] or tokens == ["const", "*"] or tokens == ["*", "const"]:
        return 1
    return 0


def find_in_dictionary(complex_token):
    t_without_pointer = complex_token
    copied = 0
    if complex_token.__contains__("*"):
        copied = 1
        t_without_pointer = complex_token.copy()
        t_without_pointer.remove("*")
    if complex_token.__contains__("const"):
        if copied == 0:
            t_without_pointer = complex_token.copy()
        t_without_pointer.remove("const")
    for token_info in token_to_type_dictionary:
        if token_info['tokens'].split(" ") == t_without_pointer:
            return token_info
    print_error_and_exit(-1, "Failed to find " + str(complex_token) + " in tokens dictionary")


def is_reserved_token(m_token):
    for r_token in reserved_tokens:
        if r_token == m_token:
            return 1
    return 0


def handle_custom_type(line_parts, line_number, struct):
    if struct is None:
        print_error_and_exit(line_number, "Illegal state exception, token received while struct is None")
        return 0
    if len(line_parts) <= 1:
        return 0
    type_index = 0
    if line_parts.__contains__("const"):
        type_index = line_parts.index("const") + 1
    v_type = line_parts[type_index]
    type_modifier = None
    index = type_index + 1
    if v_type == "enum" or v_type == "struct":
        v_type = line_parts[index]
        index += 1
    if line_parts.__contains__("*"):
        type_modifier = TypeModifier.POINTER
        index = line_parts.index("*") + 1
        if index < type_index:
            index = type_index
    process_variable(line_parts, line_number, struct, index, v_type, type_modifier)
    return 1


def handle_macros_or_enum_field(line, line_number, struct):
    if not line.__contains__("("):
        # means that mostly it's an enum field or field with custom type, so just add it as it is
        # its eligibility will be checked later.
        struct.append_field(Field("", line.replace(",", "")))
        return
    if not line.__contains__(")"):
        print_error_and_exit(line_number, "Macro should be callable and contain ')'")
        struct.append_field(Field("", line))
        return
    invoke_idx_s = line.index("(")
    invoke_idx_e = line.index(")")
    parameter = line[invoke_idx_s + 1:invoke_idx_e]
    # macro = line[:invoke_idx_s] # -> contains name of macro, but currently we're supporting only one macro.
    struct.append_field(Field(parameter, parameter + "_t", 1, TypeModifier.ARRAY, 0))


def handle_end_of_struct(line_parts):
    if len(line_parts) > 1:
        return line_parts[1].strip()
    else:
        return str(line_parts[0][1:]).strip()


def structure_names_and_line_ranges(lines):
    line_number = 1
    pre_parsed_structures = []
    current_struct_info = {}
    is_comment_open = 0
    for line in lines:
        clear_line = line.replace("\n", "").replace(";", "").strip()
        # skip empty line
        if clear_line == "":
            line_number += 1
            continue
        line_parts = split_line_into_parts(clear_line)

        # handle comments
        if line_parts[0].startswith("//"):
            line_number += 1
            continue
        if line_parts[0] == "/":
            length = len(line_parts)
            if length == 1:
                print_error_and_exit(line_number, "Expected '/' or '*', but nothing was received")
                line_number += 1
                continue
            if clear_line.endswith("*/"):
                is_comment_open = 0
                line_number += 1
                continue
            if length >= 2 and line_parts[1] == "*":
                is_comment_open = 1
                line_number += 1
                continue
            line_number += 1
            continue
        if line_parts[0] == "*":
            if is_comment_open == 1 and len(line_parts) > 1 and (
                    line_parts[1] == "/" or line_parts[len(line_parts) - 1].endswith("*/")):
                is_comment_open = 0
                line_number += 1
                continue
            if is_comment_open == 0:
                print_error_and_exit(line_number, "Wrong start token '*'")
                line_number += 1
                continue
            line_number += 1
            continue
        if is_comment_open == 1:
            line_number += 1
            continue
        # end of comments handling
        if line_parts[0] == "typedef":
            is_enum = 0
            if line_parts[1] == "enum":
                is_enum = 1
            current_struct_info = {'tokens': [], 'name': "", 'is_enum': is_enum}
            line_number += 1
            continue
        if (line_parts[0] == "enum" or line_parts[0] == "struct") and len(current_struct_info['tokens']) == 0:
            is_enum = line_parts[0] == "enum"
            current_struct_info = {'tokens': [], 'name': line_parts[1], 'is_enum': is_enum}
            line_number += 1
            continue
        if line_parts[0].startswith("}"):
            end = handle_end_of_struct(line_parts)
            if end != "":
                current_struct_info['name'] = end
            pre_parsed_structures.append(current_struct_info)
            current_struct_info = {'tokens': [], 'name': "", 'is_enum': 0}
            line_number += 1
            continue
        if line_parts[0] == "{":
            line_number += 1
            continue
        current_struct_info['tokens'].append({'line_number': line_number, 'tokens': line_parts})
        line_number += 1
    return pre_parsed_structures


def is_custom_type(v_type):
    for proto_token in proto_tokens:
        if proto_token == v_type:
            return 0
    return 1


def check_custom_types(processed_data, struct_names):
    for struct in processed_data:
        if struct.is_enum == 1:
            continue
        for field in struct.fields:
            if is_custom_type(field.proto_type) == 1:
                if not struct_names.__contains__(field.proto_type):
                    field.identified = 0


def process_lines(lines):
    processed_data = []
    pre_parsed_data = structure_names_and_line_ranges(lines)
    for pre_parsed_structure_data in pre_parsed_data:
        if parsing_structs and not parsing_structs.__contains__(pre_parsed_structure_data['name']):
            continue
        current_struct = Struct()
        current_struct.name = pre_parsed_structure_data['name']
        current_struct.is_enum = pre_parsed_structure_data['is_enum']
        for line_parts_data in pre_parsed_structure_data['tokens']:
            line_number = line_parts_data['line_number']
            line_parts = line_parts_data['tokens']
            if handle_simple_types(line_parts, line_number, current_struct) == 1:
                continue
            if handle_custom_type(line_parts, line_number, current_struct) == 1:
                continue
            handle_macros_or_enum_field(" ".join(line_parts), line_number, current_struct)
        processed_data.append(current_struct)
    if print_results == 1:
        print_parsed_data(processed_data)

    structure_names = []
    for pre_parsed in pre_parsed_data:
        structure_names.append(pre_parsed['name'])
    check_custom_types(processed_data, structure_names)
    return processed_data


def print_parsed_data(parsed_data):
    for struct in parsed_data:
        print("\t\t\t\t" + struct.name)
        struct.print()
        print("\n")


def full_file_path(file_name):
    global_vars = globals()
    temp = file_path_and_file_name(file_name)
    if global_vars['output_file_name'] is None:
        out = temp['path'] + temp['file_name'] + ".proto"
    else:
        if directoryOnly == 1:
            out = global_vars['output_file_name'] + temp['file_name'] + ".proto"
        else:
            out = global_vars['output_file_name']
    return {
        'file_path': out,
        'file_name': temp['file_name']
    }


def format_to_proto(parsed_data, proto_package):
    file_content = "syntax=\"" + protobuf_version + "\";\n\npackage " + proto_package + ";\n\n"
    for struct in parsed_data:
        message = "message "
        index = 1
        if struct.is_enum == 1:
            message = "enum "
            index = 0
        file_content += message + struct.name + " {\n"
        for field in struct.fields:
            repeated = ""
            value = ""
            prefix = ""
            if field.type_modifier is TypeModifier.ARRAY:
                repeated = "repeated "
            if field.value != "":
                value = field.value + " "
            if field.identified == 0:
                prefix = "// TODO "
            file_content += "\t" + prefix + repeated + field.proto_type + " " + value + "= " + str(index) + ";\n"
            index += 1
        file_content += "}\n\n"
    adapter_code = generate_adapter_code(parsed_data, proto_package)
    if print_results == 1:
        print(adapter_code)
    return file_content + "\n//\t\tGENERATED TRANSFORM FUNCTIONS\n\n" + adapter_code


def lowercase_first_letter(text):
    return text[:1].lower() + text[1:] if text else ''


def generate_adapter_code(parsed_data, proto_package):
    generated_message = ""
    for struct in parsed_data:
        if struct.is_enum == 1:
            continue
        original_struct_name = struct.name
        proto_struct_name = proto_package + "::" + struct.name
        variable_name = lowercase_first_letter(struct.name)
        old_variable_name = lowercase_first_letter(struct.name) + "_old"
        generated_message += "/*"
        generated_message += "\nvoid transform_" + struct.name + "(" + proto_struct_name + "* " + variable_name +\
                             ", " + original_struct_name + "* " + old_variable_name + ") {\n"
        for field in struct.fields:
            if field.identified == 0:
                continue
            if is_custom_type(field.proto_type) == 1:
                if field.type_modifier == TypeModifier.ARRAY:
                    generated_message += "\tfor(int i = 0; i < TODO length; i++) {\n"
                    generated_message += "\t\t" + proto_package + "::" + field.proto_type + "* " + field.value +\
                                         " = " + variable_name + "->add_" + field.value + "();\n"
                    generated_message += "\t\ttransform_" + field.proto_type + "(" + field.value + ", &" + \
                                         old_variable_name + "->" + field.value + "[i]);\n"
                    generated_message += "\t}\n"
                    continue
                generated_message += "\t" + proto_package + "::" + field.proto_type + " " + field.value + ";\n"
                generated_message += "\ttransform_" + field.proto_type + "(&" + field.value + ", " + \
                                     old_variable_name + "->" + field.value + ");\n"
                continue
            else:
                if field.type_modifier == TypeModifier.ARRAY:
                    size = field.array_size
                    size_hint = ""
                    if size != -1:
                        size_hint = ", possible size " + str(size)
                    generated_message += "\tfor(int i = 0; i < TODO length" + size_hint + "; i++) {\n"
                    generated_message += "\t\t" + variable_name + "->add_" + field.value + "(" + old_variable_name +\
                                         "->" + field.value + "[i]);\n"
                    generated_message += "\t}\n"
                    continue
                pointer_prefix = ""
                if field.type_modifier == TypeModifier.POINTER:
                    pointer_prefix = "*"
                generated_message += "\t" + variable_name + "->set_" + field.value + "(" + pointer_prefix + \
                                     old_variable_name + "->" + field.value + ");\n"
        generated_message += "}"
        generated_message += "\n*/\n"
    return generated_message


def start(file_name, proto_package=None):
    with open(file_name, "r") as in_file:
        parsed_data = process_lines(in_file.readlines())
        in_file.close()
        output_data = full_file_path(file_name)
        with open(output_data['file_path'], "w") as out_file:
            p = output_data['file_name']
            if proto_package is None:
                proto_package = p
            out_file.write(format_to_proto(parsed_data, proto_package))
            out_file.close()


def parse_arguments_argument(arg, single=0):
    temp = arg.split("=")[1]
    if single:
        return temp
    return temp.split(",")


def process_start_arguments(arguments):
    global_vars = globals()
    file_args = []
    for arg in arguments:
        # specifies output path
        if arg.startswith("-o="):
            try:
                output_path = parse_arguments_argument(arg, 1)
                if output_path.__contains__("."):
                    global_vars['directoryOnly'] = 0
                else:
                    global_vars['directoryOnly'] = 1
                global_vars['output_file_name'] = output_path
            except IndexError:
                global_vars['output_file_name'] = None
            continue
        # flag that stops failing program on error happened, just print an error and continue working
        if arg == "-w":
            global_vars['abort_on_error'] = 1
            continue
        # debug print, prints parsed structures to the console
        if arg == "-v":
            global_vars['print_results'] = 1
            continue
        # specifies structs that should be parsed and transformed to the proto, by default all structs in the file
        # will be processed
        if arg.startswith("-s="):
            global_vars['parsing_structs'] = parse_arguments_argument(arg)
            continue
        # specifies packages for proto files
        if arg.startswith("-p="):
            global_vars['package'] = parse_arguments_argument(arg)
            continue
        file_args.append(arg)
    return file_args


for argument in process_start_arguments(sys.argv):
    argCount += 1
    # skip first because first one it's name of the program file
    if argCount == 0:
        continue
    package = None
    if packages and len(packages) >= argCount - 1:
        package = packages[argCount - 1]
    start(argument, package)
