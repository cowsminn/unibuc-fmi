using System.Text.Json;

namespace PhotoShare.Extensions
{
    public static class SessionExtensions
    {
        public static void SetObject<T>(this ISession session, string key, T value)
        {
            session.SetString(key, JsonSerializer.Serialize(value));
        }

        public static T? GetObject<T>(this ISession session, string key)
        {
            var value = session.GetString(key);
            return value == null ? default : JsonSerializer.Deserialize<T>(value);
        }

        public static int? GetCurrentUserId(this ISession session)
        {
            return session.GetInt32("UserId");
        }

        public static void SetCurrentUserId(this ISession session, int userId)
        {
            session.SetInt32("UserId", userId);
        }

        public static bool IsLoggedIn(this ISession session)
        {
            return session.GetCurrentUserId().HasValue;
        }

        public static void ClearAuth(this ISession session)
        {
            session.Remove("UserId");
            session.Remove("UserName");
            session.Remove("UserType");
            
            // Clear any other session data that might exist
            session.Clear();
        }
    }
}