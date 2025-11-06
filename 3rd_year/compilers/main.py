keywords = {"int", "string", "if", "else", "return", "scanf", "printf", "float"}
operators = {"+", "++", "-", "--", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", "&"}
separators = {"(", ")", "{", "}", ",", ";", "[", "]"}

def is_keyword_or_id(token):
    return "keyword" if token in keywords else "identifier"

def detect_number(text, pos):
    start = pos
    decimal_found = False
    while pos < len(text) and (text[pos].isdigit() or (text[pos] == '.' and not decimal_found)):
        if text[pos] == '.':
            decimal_found = True
        pos += 1
    return text[start:pos], pos

def detect_string(text, pos, line):
    start = pos
    pos += 1
    while pos < len(text) and text[pos] != '"':
        if text[pos] == '\n':
            line += 1
        pos += 1
    return text[start + 1:pos], line, pos + 1

def detect_comment(text, pos, line):
    start = pos
    if text[pos:pos + 2] == '/*':
        pos += 2
        while pos < len(text) - 1 and text[pos:pos + 2] != '*/':
            if text[pos] == '\n':
                line += 1
            pos += 1
        return text[start:pos + 2], line, pos + 2
    elif text[pos:pos + 2] == '//':
        pos += 2
        while pos < len(text) and text[pos] != '\n':
            pos += 1
        return text[start:pos], line, pos
    return None, line, pos

def detect_operator(text, pos):
    if text[pos:pos + 2] in operators:
        return text[pos:pos + 2], pos + 2
    elif text[pos] in operators:
        return text[pos], pos + 1
    return None, pos

def detect_separator(text, pos):
    if text[pos] in separators:
        return text[pos], pos + 1
    return None, pos

def skip_whitespace(text, pos, line):
    while pos < len(text) and text[pos] in {' ', '\t', '\n'}:
        if text[pos] == '\n':
            line += 1
        pos += 1
    return pos, line

def parse_token(text):
    pos = 0
    line = 1
    tokens = []

    while pos < len(text):
        pos, line = skip_whitespace(text, pos, line)
        if pos >= len(text):
            break

        if text[pos] == '#':
            start = pos
            while pos < len(text) and text[pos] != '\n':
                pos += 1
            tokens.append((text[start:pos], line, "preprocessor directive", pos - start))

        elif text[pos].isalpha() or text[pos] == '_':
            start = pos
            while pos < len(text) and (text[pos].isalnum() or text[pos] == '_'):
                pos += 1
            token = text[start:pos]
            tokens.append((token, line, is_keyword_or_id(token), pos - start))

        elif text[pos].isdigit():
            token, pos = detect_number(text, pos)
            tokens.append((token, line, "number", len(token)))

        elif text[pos] == '"':
            token, line, pos = detect_string(text, pos, line)
            tokens.append((token, line, "string", len(token)))

        else:
            comment, line, pos = detect_comment(text, pos, line)
            if comment:
                tokens.append((comment, line, "comment", len(comment)))
            else:
                operator, pos = detect_operator(text, pos)
                if operator:
                    tokens.append((operator, line, "operator", len(operator)))
                else:
                    separator, pos = detect_separator(text, pos)
                    if separator:
                        tokens.append((separator, line, "separator", len(separator)))
                    else:
                        tokens.append((text[pos], line, "unknown", 1))
                        pos += 1
    return tokens

def analyze_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read()

    tokens = parse_token(content)

    for token, line, token_type, length in tokens:
        print(f'Token: "{token}" | Line: {line} | Type: {token_type} | Length: {length}')

if __name__ == "__main__":
    analyze_file("ex.c")


