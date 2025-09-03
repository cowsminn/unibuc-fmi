using Microsoft.EntityFrameworkCore;
using PhotoShare.Data;
using PhotoShare.Services;
using PhotoShare.Models;
using PhotoShare.Extensions;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container
builder.Services.AddRazorPages();

// Add Controllers for API endpoints
builder.Services.AddControllers();

// Add Entity Framework with MySQL
builder.Services.AddDbContext<PhotoShareContext>(options =>
{
    var connectionString = builder.Configuration.GetConnectionString("DefaultConnection");
    
    if (string.IsNullOrEmpty(connectionString))
    {
        throw new InvalidOperationException("Connection string 'DefaultConnection' not found.");
    }
    
    options.UseMySQL(connectionString, mysqlOptions =>
    {
        mysqlOptions.EnableRetryOnFailure(
            maxRetryCount: 5,
            maxRetryDelay: TimeSpan.FromSeconds(30),
            errorNumbersToAdd: null);
    });
    
    // Enable detailed errors in development
    if (builder.Environment.IsDevelopment())
    {
        options.EnableSensitiveDataLogging();
        options.EnableDetailedErrors();
    }
});

// Add session support for user authentication
builder.Services.AddSession(options =>
{
    options.IdleTimeout = TimeSpan.FromMinutes(30);
    options.Cookie.HttpOnly = true;
    options.Cookie.IsEssential = true;
    options.Cookie.SecurePolicy = CookieSecurePolicy.SameAsRequest;
    options.Cookie.SameSite = SameSiteMode.Lax;
});

// Register application services
builder.Services.AddScoped<IUserService, UserService>();
builder.Services.AddScoped<IPhotoService, PhotoService>();

// Add HTTP Context Accessor for accessing session in services
builder.Services.AddHttpContextAccessor();

// Add CORS support (useful for API calls)
builder.Services.AddCors(options =>
{
    options.AddDefaultPolicy(policy =>
    {
        policy.AllowAnyOrigin()
              .AllowAnyMethod()
              .AllowAnyHeader();
    });
});

// Add logging
builder.Services.AddLogging(logging =>
{
    logging.ClearProviders();
    logging.AddConsole();
    logging.AddDebug();
});

// Configure Kestrel server options
builder.Services.Configure<Microsoft.AspNetCore.Server.Kestrel.Core.KestrelServerOptions>(options =>
{
    options.Limits.MaxRequestBodySize = 50 * 1024 * 1024; // 50MB for file uploads
});

var app = builder.Build();

// Configure the HTTP request pipeline
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Error");
    app.UseStatusCodePagesWithRedirects("/NotFound?statusCode={0}");
    app.UseHsts();
}
else
{
    app.UseDeveloperExceptionPage();
    app.UseStatusCodePagesWithRedirects("/NotFound?statusCode={0}");
}

// Enable HTTPS redirection
app.UseHttpsRedirection();

// Enable static files (CSS, JS, images)
app.UseStaticFiles();

// Enable routing
app.UseRouting();

// Enable CORS
app.UseCors();

// Enable session BEFORE authentication/authorization
app.UseSession();

// Enable authorization (if you add it later)
app.UseAuthorization();

// Map Razor Pages
app.MapRazorPages();

// Map API Controllers
app.MapControllers();

// Database initialization and seeding
await InitializeDatabaseAsync(app);

// Start the application
app.Run();

// Database initialization method
async Task InitializeDatabaseAsync(WebApplication app)
{
    using var scope = app.Services.CreateScope();
    var services = scope.ServiceProvider;
    var logger = services.GetRequiredService<ILogger<Program>>();
    
    try
    {
        var context = services.GetRequiredService<PhotoShareContext>();
        
        // Ensure database is created
        await context.Database.EnsureCreatedAsync();
        
        // Create missing tables if they don't exist
        await EnsureTablesExistAsync(context, logger);
        
        // Seed initial data
        await SeedDataAsync(context, logger);
        
        logger.LogInformation("Database initialized successfully with MySQL.");
    }
    catch (Exception ex)
    {
        logger.LogError(ex, "An error occurred while initializing the MySQL database.");
        
        // In development, you might want to see the error
        if (app.Environment.IsDevelopment())
        {
            throw;
        }
    }
}

// Ensure all tables exist (especially PhotoLikes and CommentLikes)
async Task EnsureTablesExistAsync(PhotoShareContext context, ILogger logger)
{
    try
    {
        // Check if PhotoLikes table exists, create if not
        var photoLikesExists = await context.Database.CanConnectAsync();
        if (photoLikesExists)
        {
            try
            {
                await context.PhotoLikes.AnyAsync();
                logger.LogInformation("PhotoLikes table exists.");
            }
            catch
            {
                logger.LogWarning("PhotoLikes table missing. Creating manually...");
                await context.Database.ExecuteSqlRawAsync(@"
                    CREATE TABLE IF NOT EXISTS PhotoLikes (
                        PhotoId INT NOT NULL,
                        UserId INT NOT NULL,
                        CreatedAt DATETIME(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6),
                        PRIMARY KEY (PhotoId, UserId),
                        CONSTRAINT FK_PhotoLikes_Photos FOREIGN KEY (PhotoId) REFERENCES Photos(Id) ON DELETE CASCADE,
                        CONSTRAINT FK_PhotoLikes_Users FOREIGN KEY (UserId) REFERENCES Users(Id) ON DELETE CASCADE
                    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
                ");
            }

            try
            {
                await context.CommentLikes.AnyAsync();
                logger.LogInformation("CommentLikes table exists.");
            }
            catch
            {
                logger.LogWarning("CommentLikes table missing. Creating manually...");
                await context.Database.ExecuteSqlRawAsync(@"
                    CREATE TABLE IF NOT EXISTS CommentLikes (
                        CommentId INT NOT NULL,
                        UserId INT NOT NULL,
                        CreatedAt DATETIME(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6),
                        PRIMARY KEY (CommentId, UserId),
                        CONSTRAINT FK_CommentLikes_Comments FOREIGN KEY (CommentId) REFERENCES Comments(Id) ON DELETE CASCADE,
                        CONSTRAINT FK_CommentLikes_Users_CL FOREIGN KEY (UserId) REFERENCES Users(Id) ON DELETE CASCADE
                    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
                ");
            }
        }
    }
    catch (Exception ex)
    {
        logger.LogError(ex, "Error ensuring tables exist.");
    }
}

// Seed data method - ENHANCED
async Task SeedDataAsync(PhotoShareContext context, ILogger logger)
{
    try
    {
        // Check if we already have data
        if (await context.Users.AnyAsync())
        {
            logger.LogInformation("Database already contains data. Updating like counts and comment approval...");
            
            // Don't auto-approve all existing comments - keep their current status
            // Only update like counts
            await UpdateLikeCountsAsync(context, logger);
            return;
        }
        
        // Create admin user
        var adminUser = new User
        {
            Name = "Administrator",
            Email = builder.Configuration["AdminCredentials:Email"] ?? "admin@photoshare.ro",
            PasswordHash = BCrypt.Net.BCrypt.HashPassword(
                builder.Configuration["AdminCredentials:Password"] ?? "admin123"),
            Type = UserType.Moderator,
            Bio = "Administrator PhotoShare - Gestionez platforma și ajut comunitatea.",
            CreatedAt = DateTime.UtcNow,
            LastLogin = DateTime.UtcNow
        };
        
        context.Users.Add(adminUser);
        await context.SaveChangesAsync();
        
        // Create demo users
        var demoUsers = new List<User>
        {
            new User
            {
                Name = "Ana Popescu",
                Email = "ana@photoshare.ro",
                PasswordHash = BCrypt.Net.BCrypt.HashPassword("demo123"),
                Type = UserType.Registered,
                Bio = "Pasionată de fotografia de natură și peisaje montane.",
                CreatedAt = DateTime.UtcNow.AddDays(-45),
                LastLogin = DateTime.UtcNow.AddHours(-2)
            },
            new User
            {
                Name = "Mihai Ionescu",
                Email = "mihai@photoshare.ro",
                PasswordHash = BCrypt.Net.BCrypt.HashPassword("demo123"),
                Type = UserType.Registered,
                Bio = "Fotograf urban specialist în arhitectură și street photography.",
                CreatedAt = DateTime.UtcNow.AddDays(-30),  
                LastLogin = DateTime.UtcNow.AddHours(-5)
            },
            new User
            {
                Name = "Elena Radu",
                Email = "elena@photoshare.ro",
                PasswordHash = BCrypt.Net.BCrypt.HashPassword("demo123"),
                Type = UserType.Registered,
                Bio = "Iubesc să fotografiez animale și momentele lor spontane.",
                CreatedAt = DateTime.UtcNow.AddDays(-20),
                LastLogin = DateTime.UtcNow.AddMinutes(-30)
            }
        };
        
        context.Users.AddRange(demoUsers);
        await context.SaveChangesAsync();
        
        // Create demo photos
        var allUsers = await context.Users.ToListAsync();
        var demoPhotos = new List<Photo>
        {
            new Photo
            {
                Title = "Apus spectaculos în Carpați",
                Description = "Un apus de soare magnific surprins din vârful muntelui Omu.",
                FileName = "demo_carpati_apus.jpg",
                FilePath = "uploads/demo_carpati_apus.jpg",
                FileSize = 2048000,
                ContentType = "image/jpeg",
                Category = PhotoCategory.Natura,
                UserId = demoUsers[0].Id,
                Likes = 0, // Va fi actualizat după crearea like-urilor
                Views = 230,
                UploadedAt = DateTime.UtcNow.AddDays(-8)
            },
            new Photo
            {
                Title = "Arhitectură modernă București",
                Description = "Contrast fascinant între arhitectura modernă și cea clasică.",
                FileName = "demo_bucuresti_modern.jpg",
                FilePath = "uploads/demo_bucuresti_modern.jpg",
                FileSize = 1536000,
                ContentType = "image/jpeg",
                Category = PhotoCategory.Cladiri,
                UserId = demoUsers[1].Id,
                Likes = 0,
                Views = 145,
                UploadedAt = DateTime.UtcNow.AddDays(-5)
            },
            new Photo
            {
                Title = "Pisică în grădina de primăvară",
                Description = "Prietena mea blănoasă explorând grădina în primele zile de primăvară.",
                FileName = "demo_pisica_gradina.jpg",
                FilePath = "uploads/demo_pisica_gradina.jpg",
                FileSize = 1200000,
                ContentType = "image/jpeg",
                Category = PhotoCategory.Animale,
                UserId = demoUsers[2].Id,
                Likes = 0,
                Views = 340,
                UploadedAt = DateTime.UtcNow.AddDays(-3)
            }
        };
        
        context.Photos.AddRange(demoPhotos);
        await context.SaveChangesAsync();
        
        // Create demo comments
        var demoComments = new List<Comment>
        {
            new Comment
            {
                PhotoId = demoPhotos[0].Id,
                UserId = demoUsers[1].Id,
                Content = "Incredibil de frumos! Culorile sunt absolut spectaculoase.",
                CreatedAt = DateTime.UtcNow.AddDays(-7),
                Likes = 0,
                IsApproved = true
            },
            new Comment
            {
                PhotoId = demoPhotos[0].Id,
                UserId = demoUsers[0].Id,
                Content = "Mulțumesc! Am folosit Canon EOS R5 cu obiectiv 24-70mm.",
                CreatedAt = DateTime.UtcNow.AddDays(-7).AddHours(2),
                Likes = 0,
                IsApproved = true
            }
        };
        
        context.Comments.AddRange(demoComments);
        await context.SaveChangesAsync();
        
        // Create demo albums
        var demoAlbums = new List<Album>
        {
            new Album
            {
                Name = "Peisaje montane",
                Description = "Colecția mea de fotografii din munții României",
                UserId = demoUsers[0].Id,
                IsPublic = true,
                CreatedAt = DateTime.UtcNow.AddDays(-10),
                UpdatedAt = DateTime.UtcNow.AddDays(-1)
            }
        };
        
        context.Albums.AddRange(demoAlbums);
        await context.SaveChangesAsync();
        
        // Create demo PhotoLikes
        var demoPhotoLikes = new List<PhotoLike>
        {
            new PhotoLike { PhotoId = demoPhotos[0].Id, UserId = adminUser.Id, CreatedAt = DateTime.UtcNow.AddDays(-6) },
            new PhotoLike { PhotoId = demoPhotos[0].Id, UserId = demoUsers[1].Id, CreatedAt = DateTime.UtcNow.AddDays(-5) },
            new PhotoLike { PhotoId = demoPhotos[0].Id, UserId = demoUsers[2].Id, CreatedAt = DateTime.UtcNow.AddDays(-4) },
            new PhotoLike { PhotoId = demoPhotos[1].Id, UserId = adminUser.Id, CreatedAt = DateTime.UtcNow.AddDays(-3) },
            new PhotoLike { PhotoId = demoPhotos[2].Id, UserId = demoUsers[0].Id, CreatedAt = DateTime.UtcNow.AddDays(-2) },
            new PhotoLike { PhotoId = demoPhotos[2].Id, UserId = demoUsers[1].Id, CreatedAt = DateTime.UtcNow.AddDays(-1) }
        };
        
        context.PhotoLikes.AddRange(demoPhotoLikes);
        await context.SaveChangesAsync();
        
        // Update like counts in Photos table
        await UpdateLikeCountsAsync(context, logger);
        
        logger.LogInformation("Demo data seeded successfully in MySQL database.");
        logger.LogInformation("=== CONTURI DE TEST ===");
        logger.LogInformation("Ana: ana@photoshare.ro / demo123");
        logger.LogInformation("Mihai: mihai@photoshare.ro / demo123");
        logger.LogInformation("Elena: elena@photoshare.ro / demo123");
        logger.LogInformation("========================");
    }
    catch (Exception ex)
    {
        logger.LogError(ex, "An error occurred while seeding the MySQL database.");
    }
}

// Update like counts to match PhotoLikes table
async Task UpdateLikeCountsAsync(PhotoShareContext context, ILogger logger)
{
    try
    {
        var photos = await context.Photos.ToListAsync();
        foreach (var photo in photos)
        {
            var actualLikes = await context.PhotoLikes.CountAsync(pl => pl.PhotoId == photo.Id);
            if (photo.Likes != actualLikes)
            {
                photo.Likes = actualLikes;
                logger.LogInformation("Updated photo {PhotoId} likes from {OldCount} to {NewCount}", 
                    photo.Id, photo.Likes, actualLikes);
            }
        }
        
        var comments = await context.Comments.ToListAsync();
        foreach (var comment in comments)
        {
            var actualLikes = await context.CommentLikes.CountAsync(cl => cl.CommentId == comment.Id);
            if (comment.Likes != actualLikes)
            {
                comment.Likes = actualLikes;
            }
        }
        
        await context.SaveChangesAsync();
        logger.LogInformation("Like counts updated successfully.");
    }
    catch (Exception ex)
    {
        logger.LogError(ex, "Error updating like counts.");
    }
}