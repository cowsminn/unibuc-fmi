using PhotoShare.Models;

namespace PhotoShare.Services
{
    public interface IUserService
    {
        Task<User?> GetByEmailAsync(string email);
        Task<User?> GetByIdAsync(int id);
        Task<User> CreateUserAsync(string name, string email, string password);
        Task<bool> ValidatePasswordAsync(string email, string password);
        Task UpdateLastLoginAsync(int userId);
        Task<bool> EmailExistsAsync(string email);
    }
}