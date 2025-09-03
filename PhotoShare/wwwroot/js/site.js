// Utility functions
function showLoading(element) {
    element.innerHTML = '<span class="loading"></span> Loading...';
    element.disabled = true;
}

function hideLoading(element, originalText) {
    element.innerHTML = originalText;
    element.disabled = false;
}

// Enhanced notification system
function showNotification(message, type = 'info') {
    const notification = document.createElement('div');
    notification.className = `alert alert-${type} alert-dismissible fade show photo-notification`;
    notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        z-index: 9999;
        min-width: 300px;
        max-width: 500px;
        box-shadow: var(--shadow-lg);
        backdrop-filter: blur(10px);
        border: 1px solid var(--glass-border-dark);
    `;
    
    notification.innerHTML = `
        <i class="fas fa-${getIconForType(type)} me-2"></i>
        ${message}
        <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    `;
    
    document.body.appendChild(notification);
    
    // Auto-remove after 5 seconds
    setTimeout(() => {
        if (notification.parentNode) {
            notification.classList.remove('show');
            setTimeout(() => {
                if (notification.parentNode) {
                    notification.remove();
                }
            }, 150);
        }
    }, 5000);
}

function getIconForType(type) {
    switch(type) {
        case 'success': return 'check-circle';
        case 'error':
        case 'danger': return 'exclamation-circle';
        case 'warning': return 'exclamation-triangle';
        case 'info':
        default: return 'info-circle';
    }
}

// Photo interactions
async function likePhoto(photoId, buttonElement) {
    const originalText = buttonElement.innerHTML;
    showLoading(buttonElement);
    
    try {
        const response = await fetch(`/api/photos/${photoId}/like`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            }
        });

        if (response.ok) {
            const data = await response.json();
            showNotification(data.message, 'success');
            
            // Update likes count in UI
            const likesSpan = buttonElement.querySelector('.likes-count');
            if (likesSpan) {
                const currentLikes = parseInt(likesSpan.textContent) || 0;
                likesSpan.textContent = currentLikes + 1;
            }
        } else {
            const errorData = await response.json();
            showNotification(errorData.message || 'Eroare la apreciere!', 'danger');
        }
    } catch (error) {
        console.error('Error:', error);
        showNotification('Eroare de conexiune!', 'danger');
    } finally {
        hideLoading(buttonElement, originalText);
    }
}

// Image preview for upload
function previewImage(input) {
    if (input.files && input.files[0]) {
        const reader = new FileReader();
        
        reader.onload = function(e) {
            // Remove existing preview
            const existingPreview = document.querySelector('.image-preview');
            if (existingPreview) {
                existingPreview.remove();
            }
            
            // Create new preview
            const preview = document.createElement('div');
            preview.className = 'image-preview mt-3 fade-in';
            preview.innerHTML = `
                <div class="card">
                    <img src="${e.target.result}" class="card-img-top" style="max-height: 300px; object-fit: contain;">
                    <div class="card-body">
                        <p class="card-text text-muted">
                            <i class="fas fa-info-circle"></i>
                            ${input.files[0].name} - ${(input.files[0].size / 1024 / 1024).toFixed(2)} MB
                        </p>
                    </div>
                </div>
            `;
            
            // Insert after file input
            input.parentElement.appendChild(preview);
        };
        
        reader.readAsDataURL(input.files[0]);
    }
}

// Form validation
function validateForm(formElement) {
    const inputs = formElement.querySelectorAll('input[required], textarea[required], select[required]');
    let isValid = true;
    
    inputs.forEach(input => {
        if (!input.value.trim()) {
            input.classList.add('is-invalid');
            isValid = false;
        } else {
            input.classList.remove('is-invalid');
        }
    });
    
    return isValid;
}

// Initialize when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    // Add file input change listener
    const fileInputs = document.querySelectorAll('input[type="file"]');
    fileInputs.forEach(input => {
        input.addEventListener('change', function() {
            previewImage(this);
        });
    });
    
    const forms = document.querySelectorAll('form');
    forms.forEach(form => {
        form.addEventListener('submit', function(e) {
            if (!validateForm(this)) {
                e.preventDefault();
                showNotification('Te rog completeaza toate campurile obligatorii!', 'danger');
            }
        });
    });
    
    // Add fade-in animation to cards
    const cards = document.querySelectorAll('.card');
    cards.forEach((card, index) => {
        card.style.animationDelay = `${index * 0.1}s`;
        card.classList.add('fade-in');
    });
    
    // Smooth scroll for anchor links
    const anchorLinks = document.querySelectorAll('a[href^="#"]');
    anchorLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            const target = document.querySelector(this.getAttribute('href'));
            if (target) {
                target.scrollIntoView({
                    behavior: 'smooth',
                    block: 'start'
                });
            }
        });
    });
    
    // Initialize lazy loading
    lazyLoadImages();
});

// Lazy loading for images
function lazyLoadImages() {
    const images = document.querySelectorAll('img[data-src]');
    
    if ('IntersectionObserver' in window) {
        const imageObserver = new IntersectionObserver((entries, observer) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    const img = entry.target;
                    img.src = img.dataset.src;
                    img.classList.remove('lazy');
                    imageObserver.unobserve(img);
                }
            });
        });

        images.forEach(img => imageObserver.observe(img));
    } else {
        // Fallback for older browsers
        images.forEach(img => {
            img.src = img.dataset.src;
        });
    }
}
