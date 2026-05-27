
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
    syncEmbeddedIbom(theme);
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

// ===== MERMAID DIAGRAMS =====
function setupMermaid() {
  if (!window.mermaid) return;
  window.mermaid.initialize({
    startOnLoad: false,
    securityLevel: 'strict',
    theme: 'base',
    themeVariables: {
      background: 'var(--diagram-bg)',
      primaryColor: 'var(--diagram-node)',
      primaryTextColor: 'var(--diagram-text)',
      primaryBorderColor: 'var(--diagram-border)',
      lineColor: 'var(--diagram-line)',
      secondaryColor: 'var(--diagram-accent-soft)',
      tertiaryColor: 'var(--diagram-bg)',
      clusterBkg: 'var(--diagram-bg)',
      clusterBorder: 'var(--diagram-border)',
      edgeLabelBackground: 'var(--diagram-bg)',
      fontFamily: 'JetBrains Mono, ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace'
    }
  });
  Promise.resolve(window.mermaid.run({ querySelector: '.mermaid' }))
    .then(setupDiagramZoomControls)
    .catch(err => handleError(err, 'Mermaid render'));
}

function setupDiagramZoomControls() {
  document.querySelectorAll('.diagram-card').forEach(card => {
    const diagram = card.querySelector('.mermaid');
    const svg = diagram?.querySelector('svg');
    if (!diagram || !svg || card.querySelector('.diagram-tools')) return;

    const initialBox = getSvgViewBox(svg);
    if (!initialBox) return;

    let viewBox = { ...initialBox };
    let zoom = 1;
    const tools = document.createElement('div');
    tools.className = 'diagram-tools';
    tools.innerHTML = `
      <button class="btn btn-inline" type="button" data-zoom="out" aria-label="Zoom out" title="Zoom out">-</button>
      <button class="btn btn-inline" type="button" data-zoom="reset" aria-label="Reset zoom" title="Reset zoom">100%</button>
      <button class="btn btn-inline" type="button" data-zoom="in" aria-label="Zoom in" title="Zoom in">+</button>
      <button class="btn btn-inline" type="button" data-diagram-expand aria-label="Expand diagram" title="Expand diagram">Full</button>
    `;
    card.insertBefore(tools, diagram);

    svg.removeAttribute('width');
    svg.removeAttribute('height');
    svg.style.width = '100%';
    svg.style.height = 'auto';
    svg.style.maxWidth = 'none';

    const resetButton = tools.querySelector('[data-zoom="reset"]');
    const expandButton = tools.querySelector('[data-diagram-expand]');

    const writeViewBox = () => {
      svg.setAttribute('viewBox', `${viewBox.x} ${viewBox.y} ${viewBox.width} ${viewBox.height}`);
      diagram.dataset.zoom = String(zoom);
      if (resetButton) resetButton.textContent = `${Math.round(zoom * 100)}%`;
    };

    const setZoom = (nextZoom, anchor) => {
      const safeZoom = Math.min(4, Math.max(0.65, nextZoom));
      const cx = anchor?.x ?? viewBox.x + viewBox.width / 2;
      const cy = anchor?.y ?? viewBox.y + viewBox.height / 2;
      const nextWidth = initialBox.width / safeZoom;
      const nextHeight = initialBox.height / safeZoom;
      const rx = viewBox.width ? (cx - viewBox.x) / viewBox.width : 0.5;
      const ry = viewBox.height ? (cy - viewBox.y) / viewBox.height : 0.5;

      viewBox = {
        x: cx - nextWidth * rx,
        y: cy - nextHeight * ry,
        width: nextWidth,
        height: nextHeight
      };
      zoom = safeZoom;
      writeViewBox();
    };

    const resetView = () => {
      viewBox = { ...initialBox };
      zoom = 1;
      writeViewBox();
    };

    const toggleExpanded = () => {
      card.classList.toggle('diagram-card-expanded');
      document.body.classList.toggle('has-expanded-diagram', card.classList.contains('diagram-card-expanded'));
      if (expandButton) {
        const expanded = card.classList.contains('diagram-card-expanded');
        expandButton.textContent = expanded ? 'Close' : 'Full';
        expandButton.setAttribute('aria-label', expanded ? 'Close expanded diagram' : 'Expand diagram');
        expandButton.setAttribute('title', expanded ? 'Close expanded diagram' : 'Expand diagram');
      }
      requestAnimationFrame(writeViewBox);
    };

    tools.addEventListener('click', event => {
      const button = event.target instanceof Element ? event.target.closest('button') : null;
      if (!button) return;
      const action = button.dataset.zoom;
      if (button.matches('[data-diagram-expand]')) {
        toggleExpanded();
        return;
      }
      if (!action) return;
      if (action === 'in') setZoom(zoom + 0.25);
      if (action === 'out') setZoom(zoom - 0.25);
      if (action === 'reset') resetView();
    });

    diagram.addEventListener('wheel', event => {
      if (!event.ctrlKey && !event.metaKey) return;
      event.preventDefault();
      const anchor = getSvgPoint(svg, event);
      setZoom(zoom + (event.deltaY < 0 ? 0.18 : -0.18), anchor);
    }, { passive: false });

    let isPanning = false;
    let lastPoint = null;

    diagram.addEventListener('pointerdown', event => {
      if (zoom <= 1 || event.button !== 0) return;
      event.preventDefault();
      isPanning = true;
      lastPoint = { x: event.clientX, y: event.clientY };
      diagram.classList.add('is-panning');
      diagram.setPointerCapture?.(event.pointerId);
    });

    diagram.addEventListener('pointermove', event => {
      if (!isPanning || !lastPoint) return;
      event.preventDefault();
      const bounds = svg.getBoundingClientRect();
      const scaleX = bounds.width ? viewBox.width / bounds.width : 1;
      const scaleY = bounds.height ? viewBox.height / bounds.height : 1;
      viewBox.x -= (event.clientX - lastPoint.x) * scaleX;
      viewBox.y -= (event.clientY - lastPoint.y) * scaleY;
      lastPoint = { x: event.clientX, y: event.clientY };
      writeViewBox();
    });

    const stopPanning = event => {
      if (!isPanning) return;
      isPanning = false;
      lastPoint = null;
      diagram.classList.remove('is-panning');
      diagram.releasePointerCapture?.(event.pointerId);
    };

    diagram.addEventListener('pointerup', stopPanning);
    diagram.addEventListener('pointercancel', stopPanning);
    diagram.addEventListener('dblclick', () => {
      resetView();
    });

    document.addEventListener('keydown', event => {
      if (event.key === 'Escape' && card.classList.contains('diagram-card-expanded')) {
        toggleExpanded();
      }
    });

    writeViewBox();
  });
}

function getSvgViewBox(svg) {
  const attr = svg.getAttribute('viewBox');
  if (attr) {
    const parts = attr.trim().split(/[\s,]+/).map(Number);
    if (parts.length === 4 && parts.every(Number.isFinite)) {
      return { x: parts[0], y: parts[1], width: parts[2], height: parts[3] };
    }
  }

  const width = Number.parseFloat(svg.getAttribute('width')) || svg.clientWidth;
  const height = Number.parseFloat(svg.getAttribute('height')) || svg.clientHeight;
  if (!width || !height) return null;
  svg.setAttribute('viewBox', `0 0 ${width} ${height}`);
  return { x: 0, y: 0, width, height };
}

function getSvgPoint(svg, event) {
  const bounds = svg.getBoundingClientRect();
  const current = getSvgViewBox(svg);
  if (!bounds.width || !bounds.height || !current) return null;
  return {
    x: current.x + ((event.clientX - bounds.left) / bounds.width) * current.width,
    y: current.y + ((event.clientY - bounds.top) / bounds.height) * current.height
  };
}

// ===== EMBEDDED IBOM =====
function setupEmbeddedIbom() {
  const iframe = document.getElementById('ibom-iframe');
  if (!iframe) return;
  iframe.addEventListener('load', () => syncEmbeddedIbom(), false);
  syncEmbeddedIbom();
}

function syncEmbeddedIbom(theme) {
  const iframe = document.getElementById('ibom-iframe');
  if (!iframe) return;
  const isDark = theme ? theme === 'dark' : document.body.getAttribute('data-theme') === 'dark';
  try {
    const win = iframe.contentWindow;
    if (typeof win?.setDarkMode === 'function') {
      win.setDarkMode(isDark);
      const checkbox = win.document.getElementById('darkmodeCheckbox');
      if (checkbox) checkbox.checked = isDark;
      return;
    }
    win?.document?.getElementById('topmostdiv')?.classList.toggle('dark', isDark);
  } catch (_) {}
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

// ===== ERROR HANDLING =====
function handleError(error, context = '') {
  console.error(`Error in ${context}:`, error);
  if (context === '3D Viewer') {
    const viewer = document.getElementById('pg-viewer');
    if (viewer) {
      viewer.innerHTML = '<div class="viewer-error">Failed to load 3D model</div>';
    }
  }
}

// ===== UTILS =====
function debounce(func, wait) {
  let t; return (...args) => { clearTimeout(t); t = setTimeout(() => func(...args), wait); };
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

  // Remove old service workers and caches
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
  safe('Mermaid', setupMermaid);
  safe('Embedded IBOM', setupEmbeddedIbom);
  safe('Animations', setupAnimations);
  safe('Accessibility', setupAccessibility);

  window.copyCode = copyCode;

  console.log('Photogate V2: JS initialized successfully.');
});

window.addEventListener('error', (e) => handleError(e.error || e.message || e, 'Global'));
window.addEventListener('unhandledrejection', (e) => handleError(e.reason || e, 'Promise rejection'));

// ===== RESIZE =====
const handleResize = debounce(() => {
  document.querySelectorAll('.viewer-wrap model-viewer').forEach(viewer => {
    viewer.style.height = (window.innerWidth < 768) ? '400px' : '520px';
  });
}, 200);
window.addEventListener('resize', handleResize);
