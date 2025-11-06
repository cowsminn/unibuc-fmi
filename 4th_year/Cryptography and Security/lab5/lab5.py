import secrets
import string

def generate_password(length: int = 12) -> str:
	"""
	Generate a strong random password with uppercase, lowercase, digits, and special characters.
	"""
	if length < 10:
		length = 10
	specials = ".!$@"
	upper = string.ascii_uppercase
	lower = string.ascii_lowercase
	digits = string.digits
	all_chars = upper + lower + digits + specials

	while True:
		password_chars = [
			secrets.choice(upper),
			secrets.choice(lower),
			secrets.choice(digits),
			secrets.choice(specials),
		]
		for _ in range(length - len(password_chars)):
			password_chars.append(secrets.choice(all_chars))
		secrets.SystemRandom().shuffle(password_chars)
		password = "".join(password_chars)
		if (any(c.isupper() for c in password)
				and any(c.islower() for c in password)
				and any(c.isdigit() for c in password)
				and any(c in specials for c in password)):
			return password

def generate_urlsafe(min_length: int = 32) -> str:
	"""
	Token URI included in reset password links.
	"""
	nbytes = (min_length * 3) // 4 + 1
	token = secrets.token_urlsafe(nbytes)
	if len(token) < min_length:
		while len(token) < min_length:
			token += secrets.token_urlsafe(1)
	return token[:max(len(token), min_length)]

def generate_hex_token(n_hex_digits: int = 32) -> str:
	"""
	Generate a hexadecimal token with at least n_hex_digits hex digits.
	Typical usage: API keys, transaction identifiers, etc.
	"""
	# token_hex(nbytes) returns 2 * nbytes hex digits
	nbytes = (n_hex_digits + 1) // 2
	return secrets.token_hex(nbytes)[:n_hex_digits]

def timing_safe_compare(a, b) -> bool:
	"""
	Compare two byte sequences or strings in a timing-attack-safe way.
	"""
	if isinstance(a, str):
		a = a.encode()
	if isinstance(b, str):
		b = b.encode()
	return secrets.compare_digest(a, b)

def generate_key(key_bytes: int = 32) -> bytes:
	"""
	Generate a random key for message encryption.
	"""
	return secrets.token_bytes(key_bytes)

def hash_password_bcrypt(password: str) -> bytes:
	"""
	Hash a password for storage using bcrypt.
	Returns the hash as bytes. Requires the bcrypt package.
	Why bcrypt: adaptive slow hashing with built-in salt, recommended for password storage.
	"""
	if bcrypt is None:
		raise RuntimeError("bcrypt is not available. Install with: pip install bcrypt")
	return bcrypt.hashpw(password.encode('utf-8'), bcrypt.gensalt())

def verify_password_bcrypt(password: str, hashed: bytes) -> bool:
	"""
	Verify a password against a stored bcrypt hash.
	"""
	if bcrypt is None:
		raise RuntimeError("bcrypt is not available. Install with: pip install bcrypt")
	return bcrypt.checkpw(password.encode('utf-8'), hashed)

def demo():
	pw = generate_password(12)
	print("Generated password:", pw)
	print("Scenario: creating strong passwords for users, admin credentials, or auto-generated secrets.")

	url_token = generate_urlsafe(32)
	print("Secure URL token (>=32 characters):", url_token)
	print("Scenario: hard-to-guess URL tokens for password reset or invitation links.")

	hex_tok = generate_hex_token(32)
	print("Hex token (32 hex digits):", hex_tok)
	print("Scenario: API keys or transaction identifiers stored/verified as hex strings.")

	a = "secret_value"
	b = "secret_value"
	print("Equal (timing-safe comparison):", timing_safe_compare(a, b))

	key = generate_key(32)
	print("Generated binary key (32 bytes):", key.hex())
	print("Scenario: symmetric encryption key (AES-256) for message encryption, e.g., 100 characters.")

if __name__ == "__main__":
	demo()

# bcrypt este rezistent la bruteforce cand costul este configurat corescpunzator