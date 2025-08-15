
'use strict';

// Utility to safely isolate feature errors
function safe(label, fn) {
  try { fn(); }
  catch (err) { handleError(err, label); }
}

// ===== THEME MANAGER =====
class ThemeManager {
  constructor() {
    this.btn = document.getElementById('themeToggle');
    this.currentTheme = this.getPreferredTheme();
  }
  init() {
    this.apply(this.currentTheme);
    const mediaQuery = window.matchMedia('(prefers-color-scheme: dark)');
    mediaQuery.addEventListener?.('change', (e) => {
      if (!localStorage.getItem('theme')) {
        this.apply(e.matches ? 'dark' : 'light');
      }
    });
    if (this.btn) {
      this.btn.addEventListener('click', () => this.toggle());
    }
  }
  getPreferredTheme() {
    try {
      const stored = localStorage.getItem('theme');
      if (stored) return stored;
    } catch (_) {}
    return window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
  }
  apply(theme) {
    if (theme === 'dark') {
      document.body.setAttribute('data-theme', 'dark');
      if (this.btn) this.btn.innerHTML = '<span class="nf nf-sun" aria-hidden="true"></span> Light Mode';
    } else {
      document.body.removeAttribute('data-theme');
      if (this.btn) this.btn.innerHTML = '<span class="nf nf-moon" aria-hidden="true"></span> Dark Mode';
    }
    try { localStorage.setItem('theme', theme); } catch (_) {}
    this.currentTheme = theme;
  }
  toggle() {
    this.apply(this.currentTheme === 'dark' ? 'light' : 'dark');
  }
}

// ===== SMOOTH SCROLL =====
function setupSmoothScroll() {
  document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
      e.preventDefault();
      const sel = this.getAttribute('href');
      const target = sel ? document.querySelector(sel) : null;
      const header = document.querySelector('header');
      const headerHeight = header ? header.offsetHeight : 0;
      if (target) {
        const top = Math.max(0, target.getBoundingClientRect().top + window.scrollY - headerHeight - 20);
        window.scrollTo({ top, behavior: 'smooth' });
      }
    });
  });
}

// ===== COPY CODE =====
function copyCode(button) {
  const codeBlock = button?.nextElementSibling?.querySelector('code');
  const code = codeBlock?.textContent ?? '';
  if (!code) return;
  if (navigator.clipboard?.writeText) {
    navigator.clipboard.writeText(code).then(() => showCopySuccess(button)).catch(() => showCopyError(button));
  } else {
    const textArea = document.createElement('textarea');
    textArea.value = code;
    textArea.style.position = 'fixed';
    textArea.style.left = '-9999px';
    document.body.appendChild(textArea);
    textArea.select();
    try { document.execCommand('copy'); showCopySuccess(button); }
    catch (_) { showCopyError(button); }
    document.body.removeChild(textArea);
  }
}
function showCopySuccess(button) {
  if (!button) return;
  const original = button.textContent;
  const bg = button.style.background;
  const color = button.style.color;
  button.textContent = 'Copied!';
  button.style.background = 'var(--accent-color)';
  button.style.color = '#fff';
  setTimeout(() => {
    button.textContent = original;
    button.style.background = bg;
    button.style.color = color;
  }, 1200);
}
function showCopyError(button) {
  if (!button) return;
  const original = button.textContent;
  button.textContent = 'Failed';
  setTimeout(() => { button.textContent = original; }, 1200);
}

// ===== 3D VIEWER CONTROLS =====
function setup3DViewer() {
  const viewer = document.getElementById('pg-viewer');
  const modelButtons = document.querySelectorAll('.viewer-btn');
  if (!viewer || !modelButtons.length) return;
  const MODELS = {
    board: '3dmodels/board.glb',
    case:  '3dmodels/case.gltf'
  };
  function setActiveModel(key) {
    modelButtons.forEach(btn => {
      btn.classList.remove('active');
      btn.setAttribute('aria-pressed', 'false');
    });
    const activeBtn = document.querySelector(`.viewer-btn[data-key="${key}"]`);
    if (activeBtn) {
      activeBtn.classList.add('active');
      activeBtn.setAttribute('aria-pressed', 'true');
    }
    if (MODELS[key]) {
      viewer.src = MODELS[key];
      viewer.alt = `Photogate V2 – ${key.charAt(0).toUpperCase() + key.slice(1)} 3D`;
    }
    try { localStorage.setItem('pgv2_model', key); } catch (_) {}
  }
  modelButtons.forEach(btn => btn.addEventListener('click', () => setActiveModel(btn.dataset.key)));
  const savedModel = (typeof localStorage !== 'undefined' && localStorage.getItem('pgv2_model')) || 'case';
  setActiveModel(savedModel);
}

// ===== PRISM =====
function setupPrism() {
  if (window.Prism?.plugins?.autoloader) {
    Prism.plugins.autoloader.languages_path = 'https://cdnjs.cloudflare.com/ajax/libs/prism/1.29.0/components/';
  }
}

// ===== ANIMATIONS =====
function setupAnimations() {
  const sections = document.querySelectorAll('.section');
  if (!sections.length) return;
  if ('IntersectionObserver' in window) {
    const observer = new IntersectionObserver((entries) => {
      entries.forEach(entry => {
        if (entry.isIntersecting) {
          entry.target.style.opacity = '1';
          entry.target.style.transform = 'translateY(0)';
        }
      });
    }, { threshold: 0.1, rootMargin: '0px 0px -50px 0px' });
    sections.forEach(section => {
      section.style.opacity = '0';
      section.style.transform = 'translateY(20px)';
      section.style.transition = 'opacity 0.6s ease-out, transform 0.6s ease-out';
      observer.observe(section);
    });
  } else {
    sections.forEach(section => {
      section.style.opacity = '1';
      section.style.transform = 'none';
    });
  }
}

// ===== LOADING SPINNER =====
function showLoadingSpinner(element) {
  if (!element) return;
  const spinner = document.createElement('div');
  spinner.className = 'loading-spinner';
  spinner.innerHTML = '<div class="spinner"></div>';
  element.appendChild(spinner);
}
function hideLoadingSpinner(element) {
  const spinner = element?.querySelector('.loading-spinner');
  if (spinner) spinner.remove();
}

// ===== ERROR HANDLING =====
function handleError(error, context = '') {
  console.error(`Error in ${context}:`, error);
  if (context === '3D Viewer') {
    const viewer = document.getElementById('pg-viewer');
    if (viewer) {
      viewer.innerHTML = '<div style="display:flex;align-items:center;justify-content:center;height:100%;color:var(--text-color);opacity:.7;">Failed to load 3D model</div>';
    }
  }
}

// ===== UTILS =====
function debounce(func, wait) {
  let t; return (...args) => { clearTimeout(t); t = setTimeout(() => func(...args), wait); };
}
function throttle(func, limit) {
  let inThrottle; return function(...args){ if(!inThrottle){ func.apply(this,args); inThrottle=true; setTimeout(()=>inThrottle=false,limit); } };
}

// ===== ACCESSIBILITY =====
function setupAccessibility() {
  document.querySelectorAll('.btn').forEach(btn => {
    if (!btn.hasAttribute('tabindex')) btn.setAttribute('tabindex', '0');
    btn.addEventListener('keydown', (e) => {
      if (e.key === 'Enter' || e.key === ' ') { e.preventDefault(); btn.click(); }
    });
  });
  document.addEventListener('keydown', (e) => { if (e.key === 'Tab') document.body.classList.add('keyboard-navigation'); });
  document.addEventListener('mousedown', () => { document.body.classList.remove('keyboard-navigation'); });
}

// ===== INIT =====
document.addEventListener('DOMContentLoaded', () => {

  // REMOVE SW antigo e caches
  if ('serviceWorker' in navigator) {
    navigator.serviceWorker.getRegistrations().then(regs => {
      for (const reg of regs) reg.unregister();
    }).catch(()=>{});
    if (window.caches?.keys) {
      caches.keys().then(keys => keys.forEach(k => caches.delete(k)));
    }
  }

  safe('Theme Manager', () => { const tm = new ThemeManager(); tm.init(); });
  safe('Smooth Scroll', setupSmoothScroll);
  safe('3D Viewer', setup3DViewer);
  safe('Prism', setupPrism);
  safe('Animations', setupAnimations);
  safe('Accessibility', setupAccessibility);

  window.copyCode = copyCode;

  if (location.hostname === 'localhost' || location.hostname === '127.0.0.1') {
    safe('Performance', measurePerformance);
  }

  console.log('Photogate V2: JS initialized successfully.');
});

window.addEventListener('error', (e) => handleError(e.error || e.message || e, 'Global'));
window.addEventListener('unhandledrejection', (e) => handleError(e.reason || e, 'Promise rejection'));

window.addEventListener('load', () => {
  document.body.classList.add('loaded');
  document.querySelectorAll('.loading-spinner').forEach(spinner => {
    spinner.style.opacity = '0';
    setTimeout(() => spinner.remove(), 300);
  });
});

// ===== RESIZE =====
const handleResize = debounce(() => {
  const viewer = document.getElementById('pg-viewer');
  if (!viewer) return;
  viewer.style.height = (window.innerWidth < 768) ? '400px' : '520px';
}, 200);
window.addEventListener('resize', handleResize);

