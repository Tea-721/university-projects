# TZT贪吃的蛇 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a pixel-art healing-style snake game as a single HTML file with Web Audio, theme customization, and emotional game-over effects.

**Architecture:** Single `index.html` file with modular JS sections (AudioEngine, PixelRenderer, SnakeGame, QuoteEngine, ThemeManager). Finite state machine (MENU → PLAYING → GAME_OVER) controls game flow. Canvas for rendering, Web Audio API for music/sfx, localStorage for persistence.

**Tech Stack:** Vanilla HTML/CSS/JS, Canvas 2D, Web Audio API. Zero dependencies.

---

## File Structure

| File | Responsibility |
|------|---------------|
| `index.html` | Everything — HTML structure, CSS styles, all JS modules |

The single file is organized into these logical sections:

```
<style>          — CSS for layout, UI elements, responsive design
<canvas>         — Game canvas element
<div id="ui">   — UI overlays (menu, game-over, settings, score)
<script>         — All JavaScript:
  ├── Config         — Constants, theme definitions, quote library
  ├── AudioEngine    — Web Audio synthesis (BGM + SFX)
  ├── PixelRenderer  — Canvas drawing (grid, snake, food, effects)
  ├── QuoteEngine    — Depressive quote selection by score range
  ├── ThemeManager   — Theme state, localStorage, interpolation
  ├── SnakeGame      — State machine, game logic, input handling
  └── main           — Initialization, event listeners, game loop
```

---

### Task 1: HTML Skeleton + Canvas + Basic Rendering

**Files:**
- Create: `index.html`

- [ ] **Step 1: Create index.html with HTML structure and CSS**

```html
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>TZT贪吃的蛇</title>
<style>
  * { margin: 0; padding: 0; box-sizing: border-box; }
  body {
    background: #0a0a14;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    min-height: 100vh;
    font-family: 'Courier New', monospace;
    overflow: hidden;
    color: #e8d5b7;
  }
  #game-container {
    position: relative;
    width: 400px;
    height: 400px;
    max-width: 95vmin;
    max-height: 95vmin;
  }
  canvas {
    width: 100%;
    height: 100%;
    border: 2px solid #2a2a4a;
    border-radius: 4px;
    image-rendering: pixelated;
  }
  /* Score bar above canvas */
  #score-bar {
    display: none;
    width: 100%;
    max-width: 400px;
    justify-content: space-between;
    padding: 4px 0;
    font-size: 14px;
  }
  #score-bar .current { color: #f0c040; font-size: 18px; }
  #score-bar .high { color: #8b7e74; }
  /* Controls hint below canvas */
  #controls-hint {
    display: none;
    width: 100%;
    max-width: 400px;
    text-align: center;
    padding: 4px 0;
    font-size: 11px;
    color: #4a4a5a;
  }
  /* UI overlay layers */
  .ui-overlay {
    position: absolute;
    inset: 0;
    display: none;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    z-index: 10;
  }
  .ui-overlay.active { display: flex; }
  /* Menu */
  #menu-ui {
    background: rgba(10,10,20,0.85);
  }
  #menu-ui h1 {
    font-size: 28px;
    color: #e8d5b7;
    margin-bottom: 4px;
  }
  #menu-ui .subtitle {
    font-size: 11px;
    color: #8b7e74;
    margin-bottom: 28px;
    letter-spacing: 2px;
  }
  .btn {
    display: block;
    width: 180px;
    padding: 10px 0;
    margin: 6px 0;
    text-align: center;
    border: 2px solid;
    border-radius: 4px;
    font-family: inherit;
    font-size: 14px;
    cursor: pointer;
    background: transparent;
    transition: all 0.2s;
  }
  .btn:hover { transform: scale(1.05); }
  .btn-primary {
    background: #4a7c59;
    border-color: #6ba368;
    color: #fff;
  }
  .btn-secondary {
    border-color: #4a4a5a;
    color: #8b7e74;
  }
  .btn-small {
    width: 140px;
    padding: 8px 0;
    font-size: 12px;
  }
  #menu-high-score {
    margin-top: 20px;
    font-size: 12px;
    color: #6b5b50;
  }
  /* Game Over */
  #gameover-ui {
    background: rgba(5,5,15,0.85);
  }
  #gameover-ui h2 {
    font-size: 24px;
    color: #667788;
    margin-bottom: 12px;
  }
  #gameover-score { font-size: 14px; color: #f0c040; margin-bottom: 4px; }
  #gameover-high { font-size: 12px; color: #6b5b50; margin-bottom: 20px; }
  #gameover-quote {
    font-size: 12px;
    color: #556677;
    font-style: italic;
    max-width: 280px;
    text-align: center;
    line-height: 1.8;
    margin-bottom: 24px;
  }
  /* Settings panel */
  #settings-ui {
    background: rgba(10,10,20,0.92);
    overflow-y: auto;
    padding: 20px;
  }
  #settings-ui h2 {
    font-size: 20px;
    margin-bottom: 20px;
    color: #e8d5b7;
  }
  .settings-section { margin-bottom: 16px; }
  .settings-section h3 {
    font-size: 13px;
    color: #8b7e74;
    margin-bottom: 8px;
    text-transform: uppercase;
    letter-spacing: 1px;
  }
  .settings-options {
    display: flex;
    flex-wrap: wrap;
    gap: 8px;
  }
  .setting-chip {
    padding: 6px 12px;
    border: 2px solid #3a3a4a;
    border-radius: 4px;
    font-size: 12px;
    cursor: pointer;
    color: #8b7e74;
    transition: all 0.2s;
    background: transparent;
    font-family: inherit;
  }
  .setting-chip.selected {
    border-color: #6ba368;
    color: #e8d5b7;
  }
  .setting-chip:hover { border-color: #5a5a6a; }
  #settings-back {
    margin-top: 16px;
  }
</style>
</head>
<body>
<div id="score-bar">
  <span>得分: <span class="current" id="score-display">0</span></span>
  <span class="high">最高: <span id="high-display">0</span></span>
</div>
<div id="game-container">
  <canvas id="canvas" width="400" height="400"></canvas>
  <div id="menu-ui" class="ui-overlay active">
    <h1>TZT贪吃的蛇</h1>
    <div class="subtitle">GLUTTONOUS SNAKE</div>
    <button class="btn btn-primary" id="btn-start">▶ 开始游戏</button>
    <button class="btn btn-secondary" id="btn-settings">🎨 外观设置</button>
    <div id="menu-high-score">最高纪录: <span id="menu-high">0</span></div>
  </div>
  <div id="gameover-ui" class="ui-overlay">
    <h2>Game Over</h2>
    <div id="gameover-score">得分: 0</div>
    <div id="gameover-high">最高纪录: 0</div>
    <div id="gameover-quote"></div>
    <button class="btn btn-secondary btn-small" id="btn-restart">↻ 重新开始</button>
    <button class="btn btn-secondary btn-small" id="btn-menu">⌂ 主菜单</button>
  </div>
  <div id="settings-ui" class="ui-overlay">
    <h2>外观设置</h2>
    <div class="settings-section">
      <h3>蛇身颜色</h3>
      <div class="settings-options" id="opt-snake"></div>
    </div>
    <div class="settings-section">
      <h3>背景色</h3>
      <div class="settings-options" id="opt-bg"></div>
    </div>
    <div class="settings-section">
      <h3>网格样式</h3>
      <div class="settings-options" id="opt-grid"></div>
    </div>
    <div class="settings-section">
      <h3>食物样式</h3>
      <div class="settings-options" id="opt-food"></div>
    </div>
    <button class="btn btn-secondary btn-small" id="settings-back">返回</button>
  </div>
</div>
<div id="controls-hint">WASD / 方向键 控制移动</div>
<script>
// JS goes in subsequent tasks
</script>
</body>
</html>
```

- [ ] **Step 2: Open in browser and verify layout**

Open `index.html` in a browser. Verify:
- Dark background fills the screen
- Canvas is centered, 400×400px, with a subtle border
- Menu overlay shows title "TZT贪吃的蛇", subtitle, two buttons, and high score
- Layout is centered vertically and horizontally

- [ ] **Step 3: Commit**

```bash
git add index.html
git commit -m "feat: HTML skeleton with canvas and UI structure"
```

---

### Task 2: Config + ThemeManager + Canvas Rendering

**Files:**
- Modify: `index.html` (replace `<script>` section)

- [ ] **Step 1: Add Config and ThemeManager to the script section**

Replace the empty `<script>` tag with:

```js
<script>
// ==================== CONFIG ====================
const CONFIG = {
  GRID_SIZE: 20,
  CELL_SIZE: 20,
  CANVAS_SIZE: 400,
  INITIAL_SPEED: 150,
  MIN_SPEED: 80,
  SPEED_DECREMENT: 10,
  SPEED_INTERVAL: 10, // every N points, speed up
};

const THEMES = {
  snake: {
    '经典绿': { head: '#6ba368', body: '#4a7c59', border: '#3d6b4a', eye: '#fff' },
    '霓虹粉': { head: '#ff6b9d', body: '#e84580', border: '#c23070', eye: '#fff' },
    '冰蓝':   { head: '#6bc5f0', body: '#4a9dc5', border: '#3a7da5', eye: '#fff' },
    '暗夜紫': { head: '#a855f7', body: '#7c3aed', border: '#5b21b6', eye: '#fff' },
    '琥珀金': { head: '#f59e0b', body: '#d97706', border: '#b45309', eye: '#fff' },
    '珊瑚橙': { head: '#fb923c', body: '#ea580c', border: '#c2410c', eye: '#fff' },
  },
  bg: {
    '深海':   '#0f172a',
    '森林':   '#0f1a0f',
    '沙漠':   '#1a1510',
    '樱花':   '#1a0f15',
    '极简白': '#f5f5f5',
    '深空':   '#0a0a14',
  },
  grid: {
    '隐约':   { color: '#1e1e3a', opacity: 0.3, type: 'lines' },
    '清晰':   { color: '#2a2a4a', opacity: 0.6, type: 'lines' },
    '无网格': { color: 'transparent', opacity: 0, type: 'none' },
    '点阵':   { color: '#2a2a4a', opacity: 0.5, type: 'dots' },
  },
  food: {
    '经典苹果': { color: '#e85050', shape: 'square', glow: true },
    '星星':     { color: '#f0c040', shape: 'star', glow: true },
    '蓝莓':     { color: '#60a5fa', shape: 'circle', glow: false },
    '钻石':     { color: '#a78bfa', shape: 'diamond', glow: true },
  }
};

// ==================== THEME MANAGER ====================
class ThemeManager {
  constructor() {
    this.current = {
      snake: '经典绿',
      bg: '深空',
      grid: '隐约',
      food: '经典苹果',
    };
    this.transitioning = false;
    this.transitionProgress = 1;
    this.transitionDuration = 300; // ms
    this.prevColors = null;
    this.load();
  }

  load() {
    try {
      const saved = localStorage.getItem('tzt-theme');
      if (saved) Object.assign(this.current, JSON.parse(saved));
    } catch (e) {}
  }

  save() {
    localStorage.setItem('tzt-theme', JSON.stringify(this.current));
  }

  set(key, value) {
    this.current[key] = value;
    this.save();
  }

  get snake() { return THEMES.snake[this.current.snake]; }
  get bg() { return THEMES.bg[this.current.bg]; }
  get grid() { return THEMES.grid[this.current.grid]; }
  get food() { return THEMES.food[this.current.food]; }

  startTransition() {
    this.transitioning = true;
    this.transitionProgress = 0;
    this.prevColors = {
      bg: this.bg,
      snake: this.snake,
      food: this.food,
    };
  }

  updateTransition(dt) {
    if (!this.transitioning) return;
    this.transitionProgress += dt / this.transitionDuration;
    if (this.transitionProgress >= 1) {
      this.transitionProgress = 1;
      this.transitioning = false;
    }
  }

  lerpColor(a, b, t) {
    const parse = (c) => {
      const m = c.match(/^#([0-9a-f]{2})([0-9a-f]{2})([0-9a-f]{2})$/i);
      return m ? [parseInt(m[1],16), parseInt(m[2],16), parseInt(m[3],16)] : [0,0,0];
    };
    const ca = parse(a), cb = parse(b);
    const r = Math.round(ca[0] + (cb[0]-ca[0]) * t);
    const g = Math.round(ca[1] + (cb[1]-ca[1]) * t);
    const bl = Math.round(ca[2] + (cb[2]-ca[2]) * t);
    return `rgb(${r},${g},${bl})`;
  }

  getColor(key) {
    if (!this.transitioning || !this.prevColors) return this[key];
    const t = this.transitionProgress;
    const prev = this.prevColors[key];
    const curr = this[key];
    if (key === 'bg') return this.lerpColor(prev, curr, t);
    if (key === 'snake') {
      return {
        head: this.lerpColor(prev.head, curr.head, t),
        body: this.lerpColor(prev.body, curr.body, t),
        border: this.lerpColor(prev.border, curr.border, t),
        eye: curr.eye,
      };
    }
    if (key === 'food') {
      return { ...curr, color: this.lerpColor(prev.color, curr.color, t) };
    }
    return curr;
  }
}

// ==================== PIXEL RENDERER ====================
class PixelRenderer {
  constructor(canvas, theme) {
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');
    this.theme = theme;
    this.foodGlowPhase = 0;
    this.scorePopups = [];
    this.flashAlpha = 0;
  }

  clear() {
    const ctx = this.ctx;
    const bg = this.theme.getColor('bg');
    ctx.fillStyle = bg;
    ctx.fillRect(0, 0, CONFIG.CANVAS_SIZE, CONFIG.CANVAS_SIZE);
  }

  drawGrid() {
    const ctx = this.ctx;
    const grid = this.theme.grid;
    if (grid.type === 'none') return;

    if (grid.type === 'dots') {
      ctx.fillStyle = grid.color;
      ctx.globalAlpha = grid.opacity;
      for (let x = 0; x < CONFIG.GRID_SIZE; x++) {
        for (let y = 0; y < CONFIG.GRID_SIZE; y++) {
          ctx.beginPath();
          ctx.arc(
            x * CONFIG.CELL_SIZE + CONFIG.CELL_SIZE / 2,
            y * CONFIG.CELL_SIZE + CONFIG.CELL_SIZE / 2,
            1, 0, Math.PI * 2
          );
          ctx.fill();
        }
      }
      ctx.globalAlpha = 1;
      return;
    }

    ctx.strokeStyle = grid.color;
    ctx.globalAlpha = grid.opacity;
    ctx.lineWidth = 1;
    for (let i = 0; i <= CONFIG.GRID_SIZE; i++) {
      const pos = i * CONFIG.CELL_SIZE;
      ctx.beginPath();
      ctx.moveTo(pos, 0);
      ctx.lineTo(pos, CONFIG.CANVAS_SIZE);
      ctx.stroke();
      ctx.beginPath();
      ctx.moveTo(0, pos);
      ctx.lineTo(CONFIG.CANVAS_SIZE, pos);
      ctx.stroke();
    }
    ctx.globalAlpha = 1;
  }

  drawSnake(snake, direction) {
    const ctx = this.ctx;
    const colors = this.theme.getColor('snake');

    for (let i = snake.length - 1; i >= 0; i--) {
      const seg = snake[i];
      const x = seg.x * CONFIG.CELL_SIZE;
      const y = seg.y * CONFIG.CELL_SIZE;
      const size = CONFIG.CELL_SIZE;

      ctx.fillStyle = i === 0 ? colors.head : colors.body;
      ctx.strokeStyle = colors.border;
      ctx.lineWidth = 1;

      // Rounded rect
      const r = 3;
      ctx.beginPath();
      ctx.moveTo(x + r, y);
      ctx.lineTo(x + size - r, y);
      ctx.quadraticCurveTo(x + size, y, x + size, y + r);
      ctx.lineTo(x + size, y + size - r);
      ctx.quadraticCurveTo(x + size, y + size, x + size - r, y + size);
      ctx.lineTo(x + r, y + size);
      ctx.quadraticCurveTo(x, y + size, x, y + size - r);
      ctx.lineTo(x, y + r);
      ctx.quadraticCurveTo(x, y, x + r, y);
      ctx.closePath();
      ctx.fill();
      ctx.stroke();

      // Eyes on head
      if (i === 0) {
        ctx.fillStyle = colors.eye;
        const cx = x + size / 2;
        const cy = y + size / 2;
        const ed = 4; // eye distance from center
        const es = 2; // eye size

        let ex1, ey1, ex2, ey2;
        if (direction.x === 1) { ex1 = cx+ed-2; ey1 = cy-4; ex2 = cx+ed-2; ey2 = cy+2; }
        else if (direction.x === -1) { ex1 = cx-ed+2; ey1 = cy-4; ex2 = cx-ed+2; ey2 = cy+2; }
        else if (direction.y === -1) { ex1 = cx-4; ey1 = cy-ed+2; ex2 = cx+2; ey2 = cy-ed+2; }
        else { ex1 = cx-4; ey1 = cy+ed-2; ex2 = cx+2; ey2 = cy+ed-2; }

        ctx.beginPath();
        ctx.arc(ex1, ey1, es, 0, Math.PI * 2);
        ctx.fill();
        ctx.beginPath();
        ctx.arc(ex2, ey2, es, 0, Math.PI * 2);
        ctx.fill();
      }
    }
  }

  drawFood(food, dt) {
    const ctx = this.ctx;
    const f = this.theme.getColor('food');
    this.foodGlowPhase += dt * 0.008;
    const glowIntensity = 0.5 + 0.5 * Math.sin(this.foodGlowPhase);

    const x = food.x * CONFIG.CELL_SIZE;
    const y = food.y * CONFIG.CELL_SIZE;
    const s = CONFIG.CELL_SIZE;
    const cx = x + s / 2;
    const cy = y + s / 2;

    if (f.glow) {
      ctx.shadowColor = f.color;
      ctx.shadowBlur = 6 + 4 * glowIntensity;
    }
    ctx.fillStyle = f.color;

    switch (f.shape) {
      case 'square':
        ctx.fillRect(x + 2, y + 2, s - 4, s - 4);
        break;
      case 'circle':
        ctx.beginPath();
        ctx.arc(cx, cy, s / 2 - 3, 0, Math.PI * 2);
        ctx.fill();
        break;
      case 'diamond':
        ctx.beginPath();
        ctx.moveTo(cx, y + 2);
        ctx.lineTo(x + s - 2, cy);
        ctx.lineTo(cx, y + s - 2);
        ctx.lineTo(x + 2, cy);
        ctx.closePath();
        ctx.fill();
        break;
      case 'star':
        ctx.beginPath();
        for (let i = 0; i < 5; i++) {
          const angle = (i * 4 * Math.PI) / 5 - Math.PI / 2;
          const r = s / 2 - 3;
          const px = cx + r * Math.cos(angle);
          const py = cy + r * Math.sin(angle);
          i === 0 ? ctx.moveTo(px, py) : ctx.lineTo(px, py);
        }
        ctx.closePath();
        ctx.fill();
        break;
    }
    ctx.shadowBlur = 0;
  }

  addScorePopup(score, x, y) {
    this.scorePopups.push({
      text: '+' + score,
      x: x * CONFIG.CELL_SIZE + CONFIG.CELL_SIZE / 2,
      y: y * CONFIG.CELL_SIZE,
      alpha: 1,
      vy: -1.5,
    });
  }

  triggerFlash() {
    this.flashAlpha = 0.3;
  }

  drawOverlays(dt) {
    const ctx = this.ctx;

    // Score popups
    for (let i = this.scorePopups.length - 1; i >= 0; i--) {
      const p = this.scorePopups[i];
      p.y += p.vy;
      p.alpha -= 0.02;
      if (p.alpha <= 0) { this.scorePopups.splice(i, 1); continue; }
      ctx.globalAlpha = p.alpha;
      ctx.fillStyle = '#f0c040';
      ctx.font = '14px monospace';
      ctx.textAlign = 'center';
      ctx.fillText(p.text, p.x, p.y);
    }
    ctx.globalAlpha = 1;

    // White flash
    if (this.flashAlpha > 0) {
      ctx.fillStyle = `rgba(255,255,255,${this.flashAlpha})`;
      ctx.fillRect(0, 0, CONFIG.CANVAS_SIZE, CONFIG.CANVAS_SIZE);
      this.flashAlpha -= 0.02;
    }
  }
}
</script>
```

- [ ] **Step 2: Open in browser and verify rendering**

Open `index.html`. Verify:
- Canvas shows a dark background with subtle grid lines
- Menu overlay is still visible on top
- No JS errors in console (F12)

- [ ] **Step 3: Commit**

```bash
git add index.html
git commit -m "feat: add Config, ThemeManager, and PixelRenderer"
```

---

### Task 3: Snake Game Logic + Input Handling

**Files:**
- Modify: `index.html` (add SnakeGame class before `</script>`)

- [ ] **Step 1: Add SnakeGame class with movement, food, collision, scoring**

Add before the closing `</script>` tag:

```js
// ==================== SNAKE GAME ====================
class SnakeGame {
  constructor() {
    this.state = 'MENU'; // MENU, PLAYING, GAME_OVER
    this.snake = [];
    this.direction = { x: 1, y: 0 };
    this.nextDirection = { x: 1, y: 0 };
    this.food = { x: 0, y: 0 };
    this.score = 0;
    this.highScore = parseInt(localStorage.getItem('tzt-highscore')) || 0;
    this.speed = CONFIG.INITIAL_SPEED;
    this.tickTimer = 0;
    this.inputQueue = [];
  }

  init() {
    this.snake = [
      { x: 5, y: 10 },
      { x: 4, y: 10 },
      { x: 3, y: 10 },
    ];
    this.direction = { x: 1, y: 0 };
    this.nextDirection = { x: 1, y: 0 };
    this.score = 0;
    this.speed = CONFIG.INITIAL_SPEED;
    this.inputQueue = [];
    this.spawnFood();
    this.updateScoreDisplay();
  }

  spawnFood() {
    const occupied = new Set(this.snake.map(s => `${s.x},${s.y}`));
    let pos;
    do {
      pos = {
        x: Math.floor(Math.random() * CONFIG.GRID_SIZE),
        y: Math.floor(Math.random() * CONFIG.GRID_SIZE),
      };
    } while (occupied.has(`${pos.x},${pos.y}`));
    this.food = pos;
  }

  setDirection(dx, dy) {
    // Prevent reversing
    if (this.direction.x === -dx && this.direction.y === -dy) return;
    // Queue input to process on next tick
    this.inputQueue.push({ x: dx, y: dy });
  }

  tick() {
    // Process queued input
    if (this.inputQueue.length > 0) {
      const input = this.inputQueue.shift();
      if (!(this.direction.x === -input.x && this.direction.y === -input.y)) {
        this.direction = input;
      }
    }

    // Move
    const head = {
      x: this.snake[0].x + this.direction.x,
      y: this.snake[0].y + this.direction.y,
    };

    // Wall collision
    if (head.x < 0 || head.x >= CONFIG.GRID_SIZE ||
        head.y < 0 || head.y >= CONFIG.GRID_SIZE) {
      return this.die();
    }

    // Self collision
    for (const seg of this.snake) {
      if (seg.x === head.x && seg.y === head.y) {
        return this.die();
      }
    }

    this.snake.unshift(head);

    // Eat food
    if (head.x === this.food.x && head.y === this.food.y) {
      this.score++;
      this.updateScoreDisplay();
      renderer.addScorePopup(1, this.food.x, this.food.y);
      renderer.triggerFlash();
      const isMilestone = this.score % 10 === 0;
      audio.playEat(isMilestone);

      // Speed up
      if (this.score % CONFIG.SPEED_INTERVAL === 0 && this.speed > CONFIG.MIN_SPEED) {
        this.speed -= CONFIG.SPEED_DECREMENT;
      }

      // High score
      if (this.score > this.highScore) {
        this.highScore = this.score;
        localStorage.setItem('tzt-highscore', this.highScore);
      }

      this.spawnFood();
    } else {
      this.snake.pop();
    }
  }

  die() {
    this.state = 'GAME_OVER';
    audio.playDeath();
    audio.switchToMelancholy();
    this.showGameOver();
  }

  startGame() {
    theme.startTransition();
    this.state = 'PLAYING';
    this.init();
    this.hideAllUI();
    document.getElementById('score-bar').style.display = 'flex';
    document.getElementById('controls-hint').style.display = 'block';
    audio.switchToBGM();
  }

  showGameOver() {
    document.getElementById('gameover-score').textContent = '得分: ' + this.score;
    document.getElementById('gameover-high').textContent = '最高纪录: ' + this.highScore;
    document.getElementById('gameover-quote').textContent = quoteEngine.getQuote(this.score);
    document.getElementById('gameover-ui').classList.add('active');
    document.getElementById('score-bar').style.display = 'none';
    document.getElementById('controls-hint').style.display = 'none';
  }

  showMenu() {
    this.state = 'MENU';
    this.hideAllUI();
    document.getElementById('menu-ui').classList.add('active');
    document.getElementById('menu-high').textContent = this.highScore;
    audio.switchToBGM();
  }

  hideAllUI() {
    document.querySelectorAll('.ui-overlay').forEach(el => el.classList.remove('active'));
  }

  updateScoreDisplay() {
    document.getElementById('score-display').textContent = this.score;
    document.getElementById('high-display').textContent = this.highScore;
    document.getElementById('menu-high').textContent = this.highScore;
  }
}
```

- [ ] **Step 2: Add input event listeners and game loop**

Add after the SnakeGame class (still before `</script>`):

```js
// ==================== INITIALIZATION ====================
const canvas = document.getElementById('canvas');
const theme = new ThemeManager();
const renderer = new PixelRenderer(canvas, theme);
const audio = new AudioEngine();
const quoteEngine = new QuoteEngine();
const game = new SnakeGame();

// Input handling
document.addEventListener('keydown', (e) => {
  if (game.state !== 'PLAYING') return;
  switch (e.key) {
    case 'ArrowUp': case 'w': case 'W': e.preventDefault(); game.setDirection(0, -1); break;
    case 'ArrowDown': case 's': case 'S': e.preventDefault(); game.setDirection(0, 1); break;
    case 'ArrowLeft': case 'a': case 'A': e.preventDefault(); game.setDirection(-1, 0); break;
    case 'ArrowRight': case 'd': case 'D': e.preventDefault(); game.setDirection(1, 0); break;
  }
});

// Touch controls
let touchStartX, touchStartY;
canvas.addEventListener('touchstart', (e) => {
  e.preventDefault();
  const t = e.touches[0];
  touchStartX = t.clientX;
  touchStartY = t.clientY;
}, { passive: false });

canvas.addEventListener('touchend', (e) => {
  if (game.state !== 'PLAYING') return;
  const t = e.changedTouches[0];
  const dx = t.clientX - touchStartX;
  const dy = t.clientY - touchStartY;
  const absDx = Math.abs(dx);
  const absDy = Math.abs(dy);
  if (Math.max(absDx, absDy) < 20) return;
  if (absDx > absDy) {
    game.setDirection(dx > 0 ? 1 : -1, 0);
  } else {
    game.setDirection(0, dy > 0 ? 1 : -1);
  }
});

// Button handlers
document.getElementById('btn-start').addEventListener('click', () => {
  audio.init();
  audio.playClick();
  game.startGame();
});

document.getElementById('btn-settings').addEventListener('click', () => {
  audio.init();
  audio.playClick();
  game.hideAllUI();
  document.getElementById('settings-ui').classList.add('active');
  populateSettings();
});

document.getElementById('btn-restart').addEventListener('click', () => {
  audio.playClick();
  game.startGame();
});

document.getElementById('btn-menu').addEventListener('click', () => {
  audio.playClick();
  game.showMenu();
});

document.getElementById('settings-back').addEventListener('click', () => {
  audio.playClick();
  game.hideAllUI();
  document.getElementById('menu-ui').classList.add('active');
});

// Game loop
let lastTime = 0;
let tickAccum = 0;

function gameLoop(timestamp) {
  const dt = timestamp - lastTime;
  lastTime = timestamp;

  theme.updateTransition(dt);
  renderer.clear();
  renderer.drawGrid();

  if (game.state === 'PLAYING') {
    tickAccum += dt;
    if (tickAccum >= game.speed) {
      tickAccum -= game.speed;
      game.tick();
    }
    renderer.drawSnake(game.snake, game.direction);
    renderer.drawFood(game.food, dt);
    renderer.drawOverlays(dt);
  } else if (game.state === 'MENU') {
    // Draw decorative particles on menu
    drawMenuParticles(timestamp);
  } else if (game.state === 'GAME_OVER') {
    // Keep last frame visible under overlay
    renderer.drawSnake(game.snake, game.direction);
    renderer.drawFood(game.food, dt);
    drawRain(dt);
  }

  requestAnimationFrame(gameLoop);
}

// Menu particles
const particles = Array.from({ length: 30 }, () => ({
  x: Math.random() * CONFIG.CANVAS_SIZE,
  y: Math.random() * CONFIG.CANVAS_SIZE,
  vx: (Math.random() - 0.5) * 0.3,
  vy: (Math.random() - 0.5) * 0.3,
  size: Math.random() * 2 + 1,
  alpha: Math.random() * 0.3 + 0.1,
}));

function drawMenuParticles(t) {
  const ctx = renderer.ctx;
  particles.forEach(p => {
    p.x += p.vx;
    p.y += p.vy;
    if (p.x < 0) p.x = CONFIG.CANVAS_SIZE;
    if (p.x > CONFIG.CANVAS_SIZE) p.x = 0;
    if (p.y < 0) p.y = CONFIG.CANVAS_SIZE;
    if (p.y > CONFIG.CANVAS_SIZE) p.y = 0;
    ctx.fillStyle = `rgba(232,213,183,${p.alpha})`;
    ctx.fillRect(Math.floor(p.x), Math.floor(p.y), p.size, p.size);
  });
}

// Rain effect for game over
const rainDrops = Array.from({ length: 60 }, () => ({
  x: Math.random() * CONFIG.CANVAS_SIZE,
  y: Math.random() * CONFIG.CANVAS_SIZE,
  speed: Math.random() * 4 + 3,
  length: Math.random() * 12 + 8,
  alpha: Math.random() * 0.3 + 0.1,
}));

function drawRain(dt) {
  const ctx = renderer.ctx;
  rainDrops.forEach(r => {
    r.y += r.speed;
    if (r.y > CONFIG.CANVAS_SIZE) {
      r.y = -r.length;
      r.x = Math.random() * CONFIG.CANVAS_SIZE;
    }
    ctx.strokeStyle = `rgba(100,136,170,${r.alpha})`;
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(r.x, r.y);
    ctx.lineTo(r.x, r.y + r.length);
    ctx.stroke();
  });
}

// Start
game.showMenu();
requestAnimationFrame(gameLoop);
```

- [ ] **Step 3: Open in browser and verify game mechanics**

Open `index.html`. Verify:
- Menu shows with particles in background
- Click "开始游戏" — game starts, snake moves right
- Arrow keys / WASD change direction
- Snake grows when eating food
- Score increments
- Snake dies on wall/self collision
- "重新开始" and "主菜单" buttons work
- High score persists across page reloads (localStorage)

- [ ] **Step 4: Commit**

```bash
git add index.html
git commit -m "feat: add game logic, input handling, and game loop"
```

---

### Task 4: AudioEngine (Web Audio API)

**Files:**
- Modify: `index.html` (add AudioEngine class)

- [ ] **Step 1: Add AudioEngine class**

Add before the `SnakeGame` class:

```js
// ==================== AUDIO ENGINE ====================
class AudioEngine {
  constructor() {
    this.ctx = null;
    this.bgmGain = null;
    this.sfxGain = null;
    this.bgmNodes = [];
    this.currentBGM = 'calm'; // calm or melancholy
    this.initialized = false;
  }

  init() {
    if (this.initialized) return;
    this.ctx = new (window.AudioContext || window.webkitAudioContext)();
    this.bgmGain = this.ctx.createGain();
    this.bgmGain.gain.value = 0.15;
    this.bgmGain.connect(this.ctx.destination);
    this.sfxGain = this.ctx.createGain();
    this.sfxGain.gain.value = 0.3;
    this.sfxGain.connect(this.ctx.destination);
    this.initialized = true;
  }

  // Create a note with given frequency, type, and duration
  playNote(freq, type, duration, gainNode, startDelay = 0) {
    if (!this.initialized) return;
    const osc = this.ctx.createOscillator();
    const env = this.ctx.createGain();
    osc.type = type;
    osc.frequency.value = freq;
    env.gain.setValueAtTime(0, this.ctx.currentTime + startDelay);
    env.gain.linearRampToValueAtTime(1, this.ctx.currentTime + startDelay + 0.01);
    env.gain.linearRampToValueAtTime(0, this.ctx.currentTime + startDelay + duration);
    osc.connect(env);
    env.connect(gainNode);
    osc.start(this.ctx.currentTime + startDelay);
    osc.stop(this.ctx.currentTime + startDelay + duration);
  }

  // BGM: gentle pad loop
  startBGM(type) {
    this.stopBGM();
    this.currentBGM = type;
    if (!this.initialized) return;

    const chords = type === 'calm'
      ? [[261, 329, 392], [293, 349, 440], [329, 392, 493], [261, 329, 392]] // C major progression
      : [[261, 311, 370], [293, 349, 415], [233, 293, 349], [261, 311, 370]]; // minor progression

    const playChordLoop = (chordIdx) => {
      if (this.currentBGM !== type) return;
      const chord = chords[chordIdx % chords.length];
      const duration = type === 'calm' ? 2.5 : 3.5;

      chord.forEach(freq => {
        const osc = this.ctx.createOscillator();
        const env = this.ctx.createGain();
        osc.type = 'sine';
        osc.frequency.value = freq * (type === 'melancholy' ? 0.5 : 1);
        env.gain.setValueAtTime(0, this.ctx.currentTime);
        env.gain.linearRampToValueAtTime(0.3, this.ctx.currentTime + 0.5);
        env.gain.linearRampToValueAtTime(0, this.ctx.currentTime + duration);
        osc.connect(env);
        env.connect(this.bgmGain);
        osc.start();
        osc.stop(this.ctx.currentTime + duration);
        this.bgmNodes.push(osc);
      });

      // Simple arp
      const arpNotes = type === 'calm'
        ? [523, 587, 659, 784, 659, 587]
        : [466, 523, 587, 466, 392, 349];
      arpNotes.forEach((freq, i) => {
        this.playNote(freq, 'triangle', 0.3, this.bgmGain, i * 0.4 + 0.2);
      });

      setTimeout(() => playChordLoop(chordIdx + 1), duration * 1000);
    };

    playChordLoop(0);
  }

  stopBGM() {
    this.bgmNodes.forEach(n => { try { n.stop(); } catch(e) {} });
    this.bgmNodes = [];
  }

  switchToBGM() {
    if (!this.initialized) return;
    // Fade out current, start calm
    this.bgmGain.gain.linearRampToValueAtTime(0, this.ctx.currentTime + 1);
    setTimeout(() => {
      this.bgmGain.gain.value = 0.15;
      this.startBGM('calm');
    }, 1000);
  }

  switchToMelancholy() {
    if (!this.initialized) return;
    this.bgmGain.gain.linearRampToValueAtTime(0, this.ctx.currentTime + 1);
    setTimeout(() => {
      this.bgmGain.gain.value = 0.12;
      this.startBGM('melancholy');
    }, 500);
  }

  playEat(isMilestone = false) {
    if (!this.initialized) return;
    if (isMilestone) {
      // More elaborate ascending scale with echo
      [523, 587, 659, 784, 880, 1047].forEach((f, i) => {
        this.playNote(f, 'sine', 0.2, this.sfxGain, i * 0.1);
        this.playNote(f, 'triangle', 0.25, this.sfxGain, i * 0.1 + 0.05);
      });
    } else {
      [523, 659, 784].forEach((f, i) => {
        this.playNote(f, 'sine', 0.15, this.sfxGain, i * 0.08);
      });
    }
  }

  playDeath() {
    if (!this.initialized) return;
    this.playNote(110, 'square', 0.5, this.sfxGain);
    this.playNote(82, 'square', 0.6, this.sfxGain, 0.1);
  }

  playClick() {
    if (!this.initialized) return;
    const bufferSize = this.ctx.sampleRate * 0.03;
    const buffer = this.ctx.createBuffer(1, bufferSize, this.ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < bufferSize; i++) data[i] = Math.random() * 2 - 1;
    const source = this.ctx.createBufferSource();
    source.buffer = buffer;
    const env = this.ctx.createGain();
    env.gain.setValueAtTime(0.1, this.ctx.currentTime);
    env.gain.linearRampToValueAtTime(0, this.ctx.currentTime + 0.03);
    source.connect(env);
    env.connect(this.sfxGain);
    source.start();
  }
}
```

- [ ] **Step 2: Open in browser and verify audio**

Open `index.html`. Verify:
- Click "开始游戏" — gentle BGM starts playing
- Eat food — ascending "ding" sound
- Hit wall — low thud, melancholy music fades in
- Click "重新开始" — BGM switches back
- Click buttons — subtle click sounds
- No audio errors in console

- [ ] **Step 3: Commit**

```bash
git add index.html
git commit -m "feat: add AudioEngine with Web Audio BGM and SFX"
```

---

### Task 5: QuoteEngine + Depressive Quotes

**Files:**
- Modify: `index.html` (add QuoteEngine class)

- [ ] **Step 1: Add QuoteEngine class with 30+ quotes**

Add before the `SnakeGame` class:

```js
// ==================== QUOTE ENGINE ====================
class QuoteEngine {
  constructor() {
    this.quotes = {
      low: [
        "这么快就放弃了？和你的人生一样。",
        "连两位数都没到，你真的很稳定——稳定地失败。",
        "你甚至没给我机会展示忧郁。",
        "也许贪吃蛇不适合你，试试被吃？",
        "这条蛇都比你有毅力。",
        "连食物都懒得为你出现。",
        "你的操作像极了你对待生活的态度。",
        "这么短的蛇，连寂寞都缠不住。",
        "失败来得太快，忧郁都没来得及酝酿。",
        "你以为你在玩游戏，其实游戏在玩你。",
      ],
      mid: [
        "越努力越长，越长越容易绊倒自己。",
        "每一口食物都填补不了内心的空虚，就像每一次得分都治愈不了深夜的孤独。",
        "你跑得越快，孤独追得越紧。",
        "蛇的宿命就是不断吃、不断长、不断撞。像极了人生。",
        "得分不过是延迟了失败的到来。",
        "你咬到的每一口，都是命运早已写好的剧本。",
        "成长的代价，是失去转身的自由。",
        "你已经很长了，但没有人真正看见你。",
        "每一节身体都是一个回不去的决定。",
        "你以为前方有光，其实前方只有墙。",
      ],
      high: [
        "你已经很努力了，但有些墙是注定要撞的。",
        "最高纪录是 42，但孤独的纪录是永远。",
        "你比大多数人都强，但那又怎样？",
        "蛇越长越危险，人越强越孤单。",
        "到达顶峰才发现，高处不胜寒。",
        "你的坚持令人敬佩，但结局早已注定。",
        "所有辉煌终将归于尘土，所有得分终将归于零。",
        "你以为赢了，其实只是输得比较晚。",
        "长到极致，就是自我吞噬的开始。",
        "这个世界奖励你的努力，然后在你最骄傲时收回一切。",
      ],
    };
    this.lastIndex = { low: -1, mid: -1, high: -1 };
  }

  getQuote(score) {
    const range = score < 10 ? 'low' : score <= 30 ? 'mid' : 'high';
    const list = this.quotes[range];
    let idx;
    do { idx = Math.floor(Math.random() * list.length); } while (idx === this.lastIndex[range]);
    this.lastIndex[range] = idx;
    return '"' + list[idx] + '"';
  }
}
```

- [ ] **Step 2: Verify quotes display on game over**

Open `index.html`. Play and die. Verify:
- A quote appears on the game-over screen
- Different score ranges show different quote styles
- Quotes are wrapped in quotation marks
- Dying multiple times shows different quotes (no immediate repeats)

- [ ] **Step 3: Commit**

```bash
git add index.html
git commit -m "feat: add QuoteEngine with 30 depressive quotes by score range"
```

---

### Task 6: Settings Panel (Theme Customization UI)

**Files:**
- Modify: `index.html` (add `populateSettings()` function)

- [ ] **Step 1: Add settings panel population and interaction logic**

Add after the button event listeners:

```js
// ==================== SETTINGS UI ====================
function populateSettings() {
  const sections = [
    { key: 'snake', container: 'opt-snake', options: THEMES.snake },
    { key: 'bg', container: 'opt-bg', options: THEMES.bg },
    { key: 'grid', container: 'opt-grid', options: THEMES.grid },
    { key: 'food', container: 'opt-food', options: THEMES.food },
  ];

  sections.forEach(({ key, container, options }) => {
    const el = document.getElementById(container);
    el.innerHTML = '';
    Object.keys(options).forEach(name => {
      const chip = document.createElement('button');
      chip.className = 'setting-chip' + (theme.current[key] === name ? ' selected' : '');
      chip.textContent = name;

      // Color preview for snake and food
      if (key === 'snake') {
        chip.style.borderLeft = `6px solid ${options[name].head}`;
      } else if (key === 'bg') {
        chip.style.borderLeft = `6px solid ${options[name]}`;
      } else if (key === 'food') {
        chip.style.borderLeft = `6px solid ${options[name].color}`;
      }

      chip.addEventListener('click', () => {
        audio.playClick();
        theme.set(key, name);
        // Update selected state
        el.querySelectorAll('.setting-chip').forEach(c => c.classList.remove('selected'));
        chip.classList.add('selected');
      });

      el.appendChild(chip);
    });
  });
}
```

- [ ] **Step 2: Verify settings panel**

Open `index.html`. Verify:
- Click "外观设置" — settings panel opens
- All 4 sections show their options with color previews
- Clicking an option highlights it and saves to localStorage
- "返回" goes back to menu
- Refresh page — selected options persist
- Start game with a new theme — colors match selection

- [ ] **Step 3: Commit**

```bash
git add index.html
git commit -m "feat: add settings panel with theme customization UI"
```

---

### Task 7: Polish — Responsive Design + Game Over Enhancements

**Files:**
- Modify: `index.html` (CSS + game-over rendering enhancements)

- [ ] **Step 1: Add responsive CSS and game-over visual effects**

Add to the `<style>` section after existing styles:

```css
/* Responsive */
@media (max-width: 440px) {
  #game-container {
    width: 95vmin;
    height: 95vmin;
  }
  #score-bar, #controls-hint {
    max-width: 95vmin;
  }
  #menu-ui h1 { font-size: 22px; }
  .btn { width: 150px; font-size: 12px; }
  #settings-ui { padding: 12px; }
  .setting-chip { font-size: 11px; padding: 5px 8px; }
}

/* Game over dark overlay fade */
#gameover-ui {
  transition: background-color 0.5s;
}

/* Desaturation animation for snake on death */
@keyframes desaturate {
  to { filter: grayscale(1); }
}
```

Update the `die()` method in SnakeGame to also desaturate the canvas:

```js
die() {
  this.state = 'GAME_OVER';
  this.deathTime = performance.now();
  audio.playDeath();
  audio.switchToMelancholy();
  this.showGameOver();
}
```

Update the game-over rendering in the game loop to add a dark overlay on canvas:

```js
} else if (game.state === 'GAME_OVER') {
  // Draw game state (frozen)
  renderer.drawSnake(game.snake, game.direction);
  renderer.drawFood(game.food, dt);

  // Dark overlay that fades in
  const elapsed = performance.now() - game.deathTime;
  const overlayAlpha = Math.min(0.6, elapsed / 500 * 0.6);
  const ctx = renderer.ctx;
  ctx.fillStyle = `rgba(5,5,15,${overlayAlpha})`;
  ctx.fillRect(0, 0, CONFIG.CANVAS_SIZE, CONFIG.CANVAS_SIZE);

  // Desaturate effect - redraw snake in gray
  if (elapsed < 300) {
    const grayT = elapsed / 300;
    ctx.globalAlpha = grayT;
    ctx.filter = 'grayscale(1)';
    renderer.drawSnake(game.snake, game.direction);
    ctx.filter = 'none';
    ctx.globalAlpha = 1;
  }

  drawRain(dt);
}
```

- [ ] **Step 2: Final verification checklist**

Open `index.html` and test everything:

1. **Menu**: Title, buttons, high score, particle background
2. **Start game**: Snake moves, food appears, score increments
3. **Controls**: WASD, arrows, touch swipe all work
4. **Eating**: Snake grows, score popup, flash effect, sound
5. **Speed**: Gets faster every 10 points
6. **Death**: Dark overlay fades in, rain starts, sad music, quote appears
7. **Restart**: New game starts with current theme
8. **Main menu**: Returns to menu, no theme change
9. **Settings**: All 4 customization options work
10. **Persistence**: High score and theme survive page reload
11. **Responsive**: Works on mobile viewport (DevTools device mode)
12. **Theme transition**: Colors smoothly interpolate when starting game

- [ ] **Step 3: Commit**

```bash
git add index.html
git commit -m "feat: add responsive design and game-over visual polish"
```

---

## Summary

| Task | What it builds | Verification |
|------|---------------|-------------|
| 1 | HTML skeleton + CSS + canvas | Layout visible, menu shows |
| 2 | Config + ThemeManager + Renderer | Canvas renders grid background |
| 3 | Game logic + input + game loop | Playable game with scoring |
| 4 | AudioEngine | Music and sound effects work |
| 5 | QuoteEngine | Quotes show on game over |
| 6 | Settings panel | Theme customization works |
| 7 | Polish + responsive | Full experience on mobile + desktop |
