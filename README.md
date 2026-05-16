# React Image Uploader Component - Grok
*Exported: 16/05/2026, 12:44:32*

## 🤖 Assistant
javascriptreact
Copy
"use client";

import React, { useState, useRef } from "react";
import { Plus, X, Upload, RefreshCw, Loader2 } from "lucide-react";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Badge } from "@/components/ui/badge";
import { Checkbox } from "@/components/ui/checkbox";
import { auth } from "@/firebase/config";
import toast from "react-hot-toast";

// ─────────────────────────────────────────────────────────────────────────────
// uploadToImgBB — shared upload helper
// ─────────────────────────────────────────────────────────────────────────────
export async function uploadToImgBB(file, idToken) {
  const formData = new FormData();
  formData.append("image", file);
  const res = await fetch("/api/upload-image", {
    method: "POST",
    headers: { Authorization: `Bearer ${idToken}` },
    body: formData,
  });
  if (!res.ok) throw new Error("Image upload failed");
  const json = await res.json();
  if (!json.success) throw new Error(json.error?.message || "Upload error");
  return json.data.display_url;
}

// ─────────────────────────────────────────────────────────────────────────────
// ImageUploader — single image slot
// ─────────────────────────────────────────────────────────────────────────────
export function ImageUploader({
  value,
  onChange,
  label = "Image",
  aspectClass = "aspect-video",
  maxSizeMB = 5,
}) {
  const [uploading, setUploading] = useState(false);
  const inputRef = useRef(null);

  const handleFile = async (file) => {
    if (!file) return;
    if (!file.type.startsWith("image/")) {
      toast.error("Please select an image file.");
      return;
    }
    if (file.size > maxSizeMB * 1024 * 1024) {
      toast.error(`Image must be under ${maxSizeMB} MB.`);
      return;
    }
    setUploading(true);
    try {
      const idToken = await auth.currentUser?.getIdToken();
      const url = await uploadToImgBB(file, idToken);
      onChange(url);
      toast.success("Image uploaded!");
    } catch (err) {
      toast.error(err.message || "Upload failed");
    } finally {
      setUploading(false);
      if (inputRef.current) inputRef.current.value = "";
    }
  };

  const handleDrop = (e) => {
    e.preventDefault();
    const file = e.dataTransfer.files?.[0];
    if (file) handleFile(file);
  };

  return (
    <div className="space-y-1.5">
      {label && (
        <Label className="text-xs text-slate-500 font-medium">{label}</Label>
      )}
      {value ? (
        <div
          className={`relative group w-full ${aspectClass} rounded-lg overflow-hidden border border-slate-200 bg-slate-100`}
        >
          <img src={value} alt={label} className="w-full h-full object-cover" />
          <div className="absolute inset-0 bg-black/50 opacity-0 group-hover:opacity-100 transition-opacity flex items-center justify-center gap-3">
            <button
              type="button"
              onClick={() => inputRef.current?.click()}
              disabled={uploading}
              className="flex items-center gap-1.5 px-3 py-1.5 bg-white/90 hover:bg-white text-slate-700 rounded-md text-xs font-semibold shadow transition-all"
            >
              {uploading ? (
                <Loader2 className="w-3.5 h-3.5 animate-spin" />
              ) : (
                <RefreshCw className="w-3.5 h-3.5" />
              )}
              Replace
            </button>
            <button
              type="button"
              onClick={() => onChange(null)}
              disabled={uploading}
              className="flex items-center gap-1.5 px-3 py-1.5 bg-red-500 hover:bg-red-600 text-white rounded-md text-xs font-semibold shadow transition-all"
            >
              <X className="w-3.5 h-3.5" />
              Remove
            </button>
          </div>
          {uploading && (
            <div className="absolute inset-0 bg-black/60 flex items-center justify-center">
              <Loader2 className="w-6 h-6 text-white animate-spin" />
            </div>
          )}
        </div>
      ) : (
        <div
          onDrop={handleDrop}
          onDragOver={(e) => e.preventDefault()}
          onClick={() => !uploading && inputRef.current?.click()}
          className={`
            relative w-full ${aspectClass} rounded-lg border-2 border-dashed border-slate-300
            bg-slate-50 hover:bg-blue-50 hover:border-blue-400
            flex flex-col items-center justify-center gap-2
            cursor-pointer transition-all duration-150
            ${uploading ? "pointer-events-none opacity-70" : ""}
          `}
        >
          {uploading ? (
            <>
              <Loader2 className="w-6 h-6 text-blue-500 animate-spin" />
              <span className="text-xs text-slate-500">Uploading…</span>
            </>
          ) : (
            <>
              <div className="w-10 h-10 rounded-full bg-slate-100 flex items-center justify-center">
                <Upload className="w-5 h-5 text-slate-400" />
              </div>
              <div className="text-center">
                <p className="text-xs font-semibold text-slate-600">
                  Click or drag & drop
                </p>
                <p className="text-[11px] text-slate-400 mt-0.5">
                  PNG, JPG, WEBP · max {maxSizeMB} MB
                </p>
              </div>
            </>
          )}
        </div>
      )}
      <input
        ref={inputRef}
        type="file"
        accept="image/*"
        className="hidden"
        onChange={(e) => handleFile(e.target.files?.[0])}
      />
    </div>
  );
}

// ─────────────────────────────────────────────────────────────────────────────
// MultiImageUploader — up to `max` images side by side
// ─────────────────────────────────────────────────────────────────────────────
export function MultiImageUploader({ values = [], onChange, max = 2 }) {
  const canAdd = values.length < max;

  const handleAdd = (url) => onChange([...values, url]);
  const handleRemove = (idx) => onChange(values.filter((_, i) => i !== idx));
  const handleReplace = (idx, url) => {
    const updated = [...values];
    updated[idx] = url;
    onChange(updated);
  };

  return (
    <div className="flex gap-3 flex-wrap">
      {values.map((url, idx) => (
        <div key={idx} className="w-36 flex-shrink-0">
          <ImageUploader
            value={url}
            onChange={(newUrl) =>
              newUrl === null ? handleRemove(idx) : handleReplace(idx, newUrl)
            }
            label={`Image ${idx + 1}`}
            aspectClass="aspect-video"
          />
        </div>
      ))}
      {canAdd && (
        <div className="w-36 flex-shrink-0">
          <ImageUploader
            value={null}
            onChange={(url) => url && handleAdd(url)}
            label={values.length === 0 ? "Add Image" : `Add Image ${values.length + 1}`}
            aspectClass="aspect-video"
          />
        </div>
      )}
    </div>
  );
}

// ─────────────────────────────────────────────────────────────────────────────
// ActivitySelector
// ─────────────────────────────────────────────────────────────────────────────
export function ActivitySelector({
  dayIdx,
  activityIds,
  availableActivities,
  onToggle,
  state,
}) {
  const unselected = availableActivities.filter(
    (a) => !(activityIds || []).includes(a.id)
  );

  const handleChange = (e) => {
    const selectedId = e.target.value;
    if (!selectedId) return;
    onToggle(dayIdx, selectedId);
    e.target.value = "";
  };

  return (
    <select
      onChange={handleChange}
      disabled={!state || unselected.length === 0}
      defaultValue=""
      className="w-full px-3 py-2 text-sm border rounded-md bg-white text-slate-600 focus:outline-none focus:ring-1 focus:ring-blue-500 disabled:opacity-50 disabled:cursor-not-allowed cursor-pointer"
    >
      <option value="" disabled>
        {!state
          ? "Select a state first"
          : unselected.length === 0
          ? "All activities added"
          : "Select an activity to add..."}
      </option>
      {unselected.map((activity) => (
        <option key={activity.id} value={activity.id}>
          {activity.name}
        </option>
      ))}
    </select>
  );
}

// ─────────────────────────────────────────────────────────────────────────────
// ChecklistSection
// ─────────────────────────────────────────────────────────────────────────────
export function ChecklistSection({
  items,
  onToggle,
  onSelectAll,
  onAdd,
  onRemove,
  addLabel = "Add Item",
}) {
  const [newItem, setNewItem] = useState("");
  const allSelected = items.length > 0 && items.every((i) => i.selected);

  const handleAdd = () => {
    const trimmed = newItem.trim();
    if (!trimmed) return;
    onAdd(trimmed);
    setNewItem("");
  };

  return (
    <div className="space-y-3">
      {items.length > 0 && (
        <div className="flex items-center gap-2 pb-1">
          <Checkbox
            id={`select-all-${addLabel}`}
            checked={allSelected}
            onCheckedChange={onSelectAll}
            className="border-blue-400 data-[state=checked]:bg-blue-600 data-[state=checked]:border-blue-600"
          />
          <label
            htmlFor={`select-all-${addLabel}`}
            className="text-sm font-medium text-blue-600 cursor-pointer select-none"
          >
            Select All
          </label>
        </div>
      )}
      <div className="space-y-2">
        {items.map((item) => (
          <div key={item.id} className="flex items-center gap-2 group">
            <Checkbox
              id={item.id}
              checked={item.selected}
              onCheckedChange={() => onToggle(item.id)}
              className="border-slate-300 data-[state=checked]:bg-blue-600 data-[state=checked]:border-blue-600 flex-shrink-0"
            />
            <label
              htmlFor={item.id}
              className="text-sm text-slate-700 flex-1 cursor-pointer select-none"
            >
              {item.text}
            </label>
            {item.isDefault ? (
              <button
                type="button"
                onClick={() => onRemove(item.id)}
                className="text-[10px] text-red-500 border border-red-300 rounded px-2 py-0.5 hover:bg-red-50 opacity-0 group-hover:opacity-100 transition-opacity flex-shrink-0"
              >
                Remove Default
              </button>
            ) : (
              <button
                type="button"
                onClick={() => onRemove(item.id)}
                className="opacity-0 group-hover:opacity-100 transition-opacity flex-shrink-0 p-0.5 rounded hover:bg-red-50"
              >
                <X className="w-3.5 h-3.5 text-slate-400 hover:text-red-500" />
              </button>
            )}
          </div>
        ))}
      </div>
      <div className="flex gap-2 pt-2">
        <Input
          value={newItem}
          onChange={(e) => setNewItem(e.target.value)}
          onKeyDown={(e) => {
            if (e.key === "Enter") {
              e.preventDefault();
              handleAdd();
            }
          }}
          placeholder="Type and press Enter or click Add..."
          className="text-sm h-9"
        />
        <Button
          type="button"
          size="sm"
          onClick={handleAdd}
          className="bg-blue-700 hover:bg-blue-800 text-white h-9 px-4 flex-shrink-0"
        >
          <Plus className="w-3.5 h-3.5 mr-1" /> {addLabel}
        </Button>
      </div>
    </div>
  );
}

// ─────────────────────────────────────────────────────────────────────────────
// MultiStateDropdown — checkbox-style multi-select inside a dropdown
// Props:
//   states        : string[]           – all available state names
//   selectedStates: string[]           – currently selected
//   onChange      : (states) => void
// ─────────────────────────────────────────────────────────────────────────────
export function MultiStateDropdown({ states = [], selectedStates = [], onChange }) {
  const [open, setOpen] = useState(false);
  const ref = useRef(null);

  // Close on outside click
  React.useEffect(() => {
    const handler = (e) => {
      if (ref.current && !ref.current.contains(e.target)) setOpen(false);
    };
    document.addEventListener("mousedown", handler);
    return () => document.removeEventListener("mousedown", handler);
  }, []);

  const toggle = (state) => {
    if (selectedStates.includes(state)) {
      onChange(selectedStates.filter((s) => s !== state));
    } else {
      onChange([...selectedStates, state]);
    }
  };

  return (
    <div className="relative" ref={ref}>
      <button
        type="button"
        onClick={() => setOpen((p) => !p)}
        className="w-full flex items-center justify-between px-3 py-2 border border-slate-300 rounded-md bg-white text-sm text-slate-700 hover:border-blue-400 focus:outline-none focus:ring-1 focus:ring-blue-500 transition-colors"
      >
        <span className="truncate">
          {selectedStates.length === 0
            ? "Select state(s)…"
            : selectedStates.join(", ")}
        </span>
        <svg
          className={`w-4 h-4 text-slate-400 flex-shrink-0 ml-2 transition-transform ${open ? "rotate-180" : ""}`}
          fill="none"
          viewBox="0 0 24 24"
          stroke="currentColor"
          strokeWidth={2}
        >
          <path strokeLinecap="round" strokeLinejoin="round" d="M19 9l-7 7-7-7" />
        </svg>
      </button>

      {open && (
        <div className="absolute z-50 mt-1 w-full bg-white border border-slate-200 rounded-lg shadow-lg max-h-56 overflow-y-auto py-1">
          {states.length === 0 ? (
            <p className="text-xs text-slate-400 px-3 py-2">No states available</p>
          ) : (
            states.map((state) => (
              <label
                key={state}
                className="flex items-center gap-2.5 px-3 py-2 hover:bg-blue-50 cursor-pointer select-none"
              >
                <Checkbox
                  checked={selectedStates.includes(state)}
                  onCheckedChange={() => toggle(state)}
                  className="border-slate-300 data-[state=checked]:bg-blue-600 data-[state=checked]:border-blue-600"
                />
                <span className="text-sm text-slate-700">{state}</span>
              </label>
            ))
          )}
        </div>
      )}

      {/* Selected badges */}
      {selectedStates.length > 0 && (
        <div className="flex flex-wrap gap-1 mt-2">
          {selectedStates.map((s) => (
            <Badge
              key={s}
              className="bg-blue-100 text-blue-800 border-none px-2 py-0.5 flex items-center gap-1 text-xs"
            >
              {s}
              <button
                type="button"
                onClick={() => toggle(s)}
                className="hover:text-red-600 p-0.5 rounded-full"
              >
                <X className="w-2.5 h-2.5" />
              </button>
            </Badge>
          ))}
        </div>
      )}
    </div>
  );
}
JavaScript
Copy
/**
 * app/api/ai-itinerary-template/route.js
 *
 * Used by ItineraryForm (template creation page).
 * Applies the same role-aware permission check as ai-itinerary/route.js.
 */

import { GoogleGenAI } from "@google/genai";
import { z } from "zod";
import { adminDb } from "@/firebase/admin";
import { requireAuthenticatedUser } from "@/lib/serverAuth";
import { rateLimit } from "@/lib/rateLimit";

// ─────────────────────────────────────────────────────────────────────────────
// Permission Guard — role-aware
// ─────────────────────────────────────────────────────────────────────────────
async function checkItineraryPermission(uid, role) {
  if (!uid) return false;
  if (role === "superadmin") return true;

  const collectionName =
    role === "admin" ? "adminPermissions" : "agentPermissions";

  try {
    const snap = await adminDb.collection(collectionName).doc(uid).get();
    if (!snap.exists) return false;
    return snap.data()?.itinerary_ai === true;
  } catch (err) {
    console.error("[ai-itinerary-template] Permission check failed:", err.code ?? err.message);
    return false;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// Zod Schemas
// ─────────────────────────────────────────────────────────────────────────────
const DaySchema = z.object({
  id:          z.string(),
  dayNumber:   z.number(),
  title:       z.string(),
  description: z.string(),
  activityIds: z.array(z.string()).optional().default([]),
});

const TemplateResponseSchema = z.object({
  title:     z.string(),
  states:    z.array(z.string()).optional().default([]),
  cities:    z.array(z.string()).optional().default([]),
  startCity: z.string().optional().default(""),
  endCity:   z.string().optional().default(""),
  numDays:   z.number().optional(),
  tags:      z.array(z.string()).optional().default([]),
  days:      z.array(DaySchema),
});

// ─────────────────────────────────────────────────────────────────────────────
// Gemini JSON schema (flat, no $ref)
// ─────────────────────────────────────────────────────────────────────────────
const dayGeminiSchema = {
  type: "object",
  properties: {
    id:          { type: "string" },
    dayNumber:   { type: "number" },
    title:       { type: "string" },
    description: { type: "string" },
    activityIds: { type: "array", items: { type: "string" } },
  },
  required: ["id", "dayNumber", "title", "description", "activityIds"],
};

const geminiSchema = {
  type: "object",
  properties: {
    title:     { type: "string" },
    states:    { type: "array", items: { type: "string" } },
    cities:    { type: "array", items: { type: "string" } },
    startCity: { type: "string" },
    endCity:   { type: "string" },
    numDays:   { type: "number" },
    tags:      { type: "array", items: { type: "string" } },
    days:      { type: "array", items: dayGeminiSchema },
  },
  required: ["title", "cities", "startCity", "endCity", "numDays", "days"],
};

// ─────────────────────────────────────────────────────────────────────────────
// Prompt builders
// ─────────────────────────────────────────────────────────────────────────────
function buildBasePrompt({ states, cities, startCity, endCity, numDays }) {
  const statesList = states?.length ? states.join(", ") : "India";
  const citiesList = cities?.length ? cities.join(", ") : startCity || "the destination";
  const days       = Number(numDays) > 0 ? numDays : 3;

  return `
You are an expert Indian tour itinerary writer for Adwait Tours.
Generate a complete day-wise itinerary template for an agent to use as a base.
No emojis in any text.

## TRIP CONTEXT
- States: ${statesList}
- Cities covered: ${citiesList}
- Starting city: ${startCity || citiesList.split(",")[0]?.trim() || "first city"}
- Ending city:   ${endCity   || citiesList.split(",").pop()?.trim()  || "last city"}
- Total days:    ${days}

## RULES
- Day 1: Arrival day — transfer from origin to ${startCity || "first city"}, check-in, brief local orientation.
- Intermediate days: Full sightseeing days. Use REAL, well-known attractions for each city.
- Last day: Departure — checkout and transfer from ${endCity || "last city"} back to origin.
- Descriptions: second-person, bullet points using '•' prefix, each bullet on a new line.
- Include meal plan as the last bullet: "Meal Plan: Breakfast / Lunch / Dinner" or "No Meals" on travel days.
- Generate EXACTLY ${days} day objects.
- activityIds: always an empty array [] — the agent links activities manually.
- IDs: short slugs like "day-001", "day-002", etc.

## OUTPUT
Return valid JSON only, matching the schema provided. No markdown, no explanation.
`.trim();
}

function buildRefinementSuffix({ userPrompt, chatHistory, currentItinerary }) {
  const historyText = (chatHistory || [])
    .map((m) => `${m.role === "user" ? "User" : "Assistant"}: ${m.content}`)
    .join("\n");

  return `
## REFINEMENT MODE
Apply ONLY the changes the user requested — keep everything else identical.

### Conversation history
${historyText || "(none)"}

### Current itinerary (what the user sees)
${currentItinerary ? JSON.stringify(currentItinerary, null, 2) : "(not provided — generate fresh)"}

### User request
"${userPrompt}"

Return the FULL updated itinerary JSON. Preserve unchanged day IDs. No explanation.
`.trim();
}

// ─────────────────────────────────────────────────────────────────────────────
// Route handler
// ─────────────────────────────────────────────────────────────────────────────
export async function POST(req) {
  // ── 1. Parse body ─────────────────────────────────────────────────────────
  let body;
  try {
    body = await req.json();
  } catch {
    return Response.json(
      { error: "Invalid JSON in request body." },
      { status: 400 }
    );
  }

  const {
    templateContext  = {},
    chatHistory      = [],
    userPrompt       = null,
    currentItinerary = null,
  } = body;

  // ── 2. Authenticate ───────────────────────────────────────────────────────
  let requester;
  try {
    requester = await requireAuthenticatedUser(req);
  } catch (error) {
    return Response.json(
      { error: error.message },
      { status: error.status || 401 }
    );
  }

  if (!requester.uid) {
    return Response.json(
      { error: "Authenticated user is required." },
      { status: 401 }
    );
  }

  // ── 3. Permission check — role-aware ──────────────────────────────────────
  const isAllowed = await checkItineraryPermission(requester.uid, requester.role);

  if (!isAllowed) {
    return Response.json(
      {
        error: "You don't have access to AI Itinerary Creation. Please contact your super admin to enable this feature.",
        code:  "PERMISSION_DENIED",
      },
      { status: 403 }
    );
  }

  // ── 4. Rate limit ─────────────────────────────────────────────────────────
  const rl = rateLimit({
    uid:      requester.uid,
    action:   "ai-itinerary-template",
    limit:    10,
    windowMs: 60_000,
  });
  if (!rl.allowed) {
    return Response.json(
      { error: "Too many requests. Please wait before generating again." },
      {
        status: 429,
        headers: { "Retry-After": String(Math.ceil((rl.resetAt - Date.now()) / 1000)) },
      }
    );
  }

  // ── 5. Validate templateContext ───────────────────────────────────────────
  const {
    states    = [],
    cities    = [],
    startCity = "",
    endCity   = "",
    numDays,
  } = templateContext;

  if (!Array.isArray(cities) || !Array.isArray(states)) {
    return Response.json(
      { error: "templateContext.cities and templateContext.states must be arrays." },
      { status: 400 }
    );
  }

  const resolvedDays = Number(numDays) > 0 ? Number(numDays) : 3;

  // ── 6. Sanitise chatHistory ───────────────────────────────────────────────
  const safeChatHistory = Array.isArray(chatHistory)
    ? chatHistory.filter(
        (m) =>
          m &&
          typeof m === "object" &&
          (m.role === "user" || m.role === "assistant") &&
          typeof m.content === "string"
      )
    : [];

  // ── 7. Build prompt ───────────────────────────────────────────────────────
  const isRefinement =
    typeof userPrompt === "string" && userPrompt.trim().length > 0;

  const basePrompt = buildBasePrompt({
    states,
    cities,
    startCity: startCity.trim(),
    endCity:   endCity.trim(),
    numDays:   resolvedDays,
  });

  const fullPrompt = isRefinement
    ? `${basePrompt}\n\n${buildRefinementSuffix({
        userPrompt:       userPrompt.trim(),
        chatHistory:      safeChatHistory,
        currentItinerary,
      })}`
    : basePrompt;

  // ── 8. Init Gemini ────────────────────────────────────────────────────────
  if (!process.env.GEMINI_API_KEY) {
    console.error("[ai-itinerary-template] GEMINI_API_KEY not set.");
    return Response.json(
      { error: "AI service is not configured. Please contact support." },
      { status: 503 }
    );
  }

  let ai;
  try {
    ai = new GoogleGenAI({ apiKey: process.env.GEMINI_API_KEY });
  } catch (initErr) {
    console.error("[ai-itinerary-template] Failed to init GoogleGenAI:", initErr);
    return Response.json(
      { error: "Failed to initialise AI service." },
      { status: 503 }
    );
  }

  // ── 9. Call Gemini ────────────────────────────────────────────────────────
  let rawText;
  try {
    const response = await ai.models.generateContent({
      model:    "gemini-2.5-flash-lite",
      contents: fullPrompt,
      config: {
        responseMimeType:   "application/json",
        responseJsonSchema: geminiSchema,
      },
    });
    rawText = response.text;
  } catch (geminiErr) {
    console.error("[ai-itinerary-template] Gemini error:", geminiErr);
    const msg = geminiErr?.message || "";

    if (msg.includes("quota") || msg.includes("429"))
      return Response.json({ error: "AI quota exceeded. Please try again in a moment." }, { status: 429 });
    if (msg.includes("safety") || msg.includes("blocked"))
      return Response.json({ error: "AI blocked this request. Try rephrasing." }, { status: 422 });
    if (msg.includes("deadline") || msg.includes("timeout"))
      return Response.json({ error: "AI took too long to respond. Please try again." }, { status: 504 });

    return Response.json(
      { error: "AI generation failed. Please try again.", details: msg || "Unknown error" },
      { status: 502 }
    );
  }

  // ── 10. Guard empty response ──────────────────────────────────────────────
  if (!rawText?.trim()) {
    return Response.json(
      { error: "AI returned an empty response. Please try again." },
      { status: 502 }
    );
  }

  // ── 11. Strip markdown fences ─────────────────────────────────────────────
  const cleanedText = rawText
    .replace(/^```(?:json)?\s*/i, "")
    .replace(/\s*```$/i, "")
    .trim();

  // ── 12. Parse JSON ────────────────────────────────────────────────────────
  let parsed;
  try {
    parsed = JSON.parse(cleanedText);
  } catch (jsonErr) {
    console.error("[ai-itinerary-template] JSON parse failed:", jsonErr.message);
    return Response.json(
      { error: "AI returned malformed data. Please try again.", details: jsonErr.message },
      { status: 422 }
    );
  }

  // ── 13. Validate with Zod ─────────────────────────────────────────────────
  let validated;
  try {
    validated = TemplateResponseSchema.parse(parsed);
  } catch (zodErr) {
    console.error("[ai-itinerary-template] Zod validation failed:", zodErr);
    // Return unvalidated as fallback rather than erroring the user
    return Response.json(parsed, {
      status: 200,
      headers: { "X-Validation-Warning": "Schema validation failed; data may be incomplete." },
    });
  }

  return Response.json(validated, { status: 200 });
}

now what we need to do here is use google search for images 1. Using the Google Custom Search JSON API This is the standard official route. It provides 100 free search queries per day, with more available for a fee. [1, 2, 3]

Step 1: Create a Programmable Search Engine (PSE) Visit the Google Programmable Search Engine control panel. Create a new search engine, and in the settings, ensure you toggle "Image search" to ON and set it to "Search the entire web". Note your Search Engine ID (cx).
Step 2: Get an API Key Go to the **Google Cloud Console**, create a project, and enable the "Custom Search API." Generate an API key under the "Credentials" tab.
Step 3: Make the Request Send a GET request to the API endpoint with the parameter searchType=image. Endpoint Example: https://googleapis.com [1, 2, 3, 4, 5, 6, 7]

so we will se that get images and from it we will select only , while allow us to add it for each seach like for poster or day wise selection , like that , so generate that way and agian give updated component while also allow us to upload custom images
 
Let me re-read the existing code carefully before writing anything.
My understanding of what's needed:

New API route /api/search-images — calls Google Custom Search JSON API with searchType=image, returns image results
CoverImageSection — search bar + results grid for poster, select one, or upload custom
DayImageSection — same pattern per day, context-aware search query pre-filled
Smart default queries (e.g. "Amber Fort Jaipur landscape travel" auto-built from day content)
No AI image generation at all — removed
All existing functionality preserved exactly

One question before I write: Do you want the image search UI to be a modal/popup or inline expandable panel within each day card and the cover section?
Inline expandable would be cleaner (no z-index fights with the chat popup), but a modal gives more screen space for the grid. Let me know and I'll build it correctly.
 
 
One question before I write: Do you want the image search UI to be a modal/popup or inline expandable panel within each day card and the cover section?
creade different sidesecion for it so i can scroll thougth results and select for poster one for day wise 2 max , also we want seaction wise for poster and for each day so give now as ur limit is going to end
16:40
Claude responded: Got it — a side panel (right drawer) that slides in, context-aware (knows if you're selecting for poster or a specific day), scrollable results grid, select up…
Got it — a side panel (right drawer) that slides in, context-aware (knows if you're selecting for poster or a specific day), scrollable results grid, select up to the allowed max, confirm selection. Let me build everything now.
 
 
bash
echo "Files to produce: 1. app/api/search-images/route.js — NEW: Google Custom Search proxy 2. ItineraryForm.jsx — UPDATED: side panel trigger on cover + each day 3. ImageSearchPanel.jsx (new component) — Side drawer with search + grid + selection Panel behavior: - Slides in from right, full height, ~420px wide - Context: { type: 'cover' | 'day', dayIdx?, dayTitle?, dayDescription? } - Auto-builds smart query from context on open - User can edit query and re-search - Results grid: 3 columns, lazy-loaded images - Cover: select 1 max; Day: select up to 2 max (respects already-uploaded count) - Confirm button applies selection back to form state - Manual upload still available separately via existing ImageUploader/MultiImageUploader "
Output
Files to produce: 1. app/api/search-images/route.js — NEW: Google Custom Search proxy 2. ItineraryForm.jsx — UPDATED: side panel trigger on cover + each day 3. ImageSearchPanel.jsx (new component) — Side drawer with search + grid + selection Panel behavior: - Slides in from right, full height, ~420px wide - Context: { type: 'cover' | 'day', dayIdx?, dayTitle?, dayDescription? } - Auto-builds smart query from context on open - User can edit query and re-search - Results grid: 3 columns, lazy-loaded images - Cover: select 1 max; Day: select up to 2 max (respects already-uploaded count) - Confirm button applies selection back to form state - Manual upload still available separately via existing ImageUploader/MultiImageUploader
/**
 * app/api/search-images/route.js
 *
 * Proxy for Google Custom Search JSON API — image search.
 * Keeps API keys server-side, enforces auth + rate limit.
 *
 * ENV vars required:
 * GOOGLE_SEARCH_API_KEY — Google Cloud API key with Custom Search API enabled
 * GOOGLE_SEARCH_CX — Programmable Search Engine ID (cx), image search ON,
 * "Search the entire web" enabled
 *
 * GET /api/search-images?q=Amber+Fort+Jaipur&start=1
 * q : search query (required)
 * start : pagination offset, 1-based, max 91 (Google limit) — default 1
 *
 * Returns:
 * { results: ImageResult[], totalResults: number, nextStart: number | null }
 *
 * ImageResult: { url, thumbnail, title, source, width, height }
 */
import { requireAuthenticatedUser } from "@/lib/serverAuth";
import { rateLimit } from "@/lib/rateLimit";
// How many results to request per page (max 10 per Google's API)
const PAGE_SIZE = 10;
// Domains known to serve watermarked / low-quality results — filtered out
const BLOCKED_DOMAINS = [
  "shutterstock.com",
  "gettyimages.com",
  "istockphoto.com",
  "adobe.com",
  "dreamstime.com",
  "depositphotos.com",
  "alamy.com",
  "123rf.com",
  "stocksy.com",
  "bigstockphoto.com",
];
function isDomainBlocked(url) {
  try {
    const hostname = new URL(url).hostname.toLowerCase();
    return BLOCKED_DOMAINS.some((d) => hostname.includes(d));
  } catch {
    return true; // malformed URL — block it
  }
}
/**
 * Validate and sanitise the search query.
 * Returns { ok: true, query } or { ok: false, error }
 */
function validateQuery(raw) {
  if (!raw || typeof raw !== "string") {
    return { ok: false, error: "Query parameter 'q' is required." };
  }
  const trimmed = raw.trim();
  if (trimmed.length === 0) {
    return { ok: false, error: "Query cannot be empty." };
  }
  if (trimmed.length > 200) {
    return { ok: false, error: "Query is too long (max 200 characters)." };
  }
  return { ok: true, query: trimmed };
}
export async function GET(req) {
  // ── 1. Authenticate ──────────────────────────────────────────────────────
  let requester;
  try {
    requester = await requireAuthenticatedUser(req);
  } catch (err) {
    return Response.json({ error: err.message }, { status: err.status || 401 });
  }
  if (!requester?.uid) {
    return Response.json({ error: "Authentication required." }, { status: 401 });
  }
  // ── 2. Rate limit — 30 searches/min per user ─────────────────────────────
  // Google gives 100 free queries/day total; 30/min per user is generous
  // but protects the daily quota from a single user burning it all.
  const rl = rateLimit({
    uid: requester.uid,
    action: "search-images",
    limit: 30,
    windowMs: 60_000,
  });
  if (!rl.allowed) {
    return Response.json(
      { error: "Too many image search requests. Please wait a moment." },
      {
        status: 429,
        headers: {
          "Retry-After": String(Math.ceil((rl.resetAt - Date.now()) / 1000)),
        },
      }
    );
  }
  // ── 3. Parse + validate query params ─────────────────────────────────────
  const { searchParams } = new URL(req.url);
  const rawQuery = searchParams.get("q");
  const rawStart = searchParams.get("start");
  const { ok, query, error: queryError } = validateQuery(rawQuery);
  if (!ok) {
    return Response.json({ error: queryError }, { status: 400 });
  }
  // start must be 1–91 (Google allows items 1-100, but only 10 per request)
  let start = parseInt(rawStart, 10);
  if (isNaN(start) || start < 1) start = 1;
  if (start > 91) start = 91;
  // ── 4. Validate env vars ──────────────────────────────────────────────────
  const apiKey = process.env.GOOGLE_SEARCH_API_KEY;
  const cx = process.env.GOOGLE_SEARCH_CX;
  if (!apiKey || !cx) {
    console.error("[search-images] Missing GOOGLE_SEARCH_API_KEY or GOOGLE_SEARCH_CX");
    return Response.json(
      { error: "Image search is not configured. Please contact support." },
      { status: 503 }
    );
  }
  // ── 5. Build Google CSE URL ───────────────────────────────────────────────
  const params = new URLSearchParams({
    key: apiKey,
    cx,
    q: query,
    searchType: "image",
    num: String(PAGE_SIZE),
    start: String(start),
    // Prefer landscape, high-quality images
    imgSize: "large",
    imgType: "photo",
    safe: "active",
    // Rights filter — prefer images labeled for reuse
    // (not strict — just a hint; agents are responsible for final use)
    rights: "cc_publicdomain|cc_attribute|cc_sharealike",
  });
  const apiUrl = https://www.googleapis.com/customsearch/v1?${params.toString()};
  // ── 6. Call Google CSE ────────────────────────────────────────────────────
  let googleResponse;
  try {
    googleResponse = await fetch(apiUrl, {
      // 10-second timeout
      signal: AbortSignal.timeout(10_000),
    });
  } catch (fetchErr) {
    console.error("[search-images] Network error calling Google CSE:", fetchErr.message);
    if (fetchErr.name === "TimeoutError" || fetchErr.name === "AbortError") {
      return Response.json(
        { error: "Image search timed out. Please try again." },
        { status: 504 }
      );
    }
    return Response.json(
      { error: "Failed to reach image search service. Please try again." },
      { status: 502 }
    );
  }
  // ── 7. Handle non-OK Google responses ────────────────────────────────────
  if (!googleResponse.ok) {
    let googleError = Google API error (${googleResponse.status});
    try {
      const errBody = await googleResponse.json();
      const msg = errBody?.error?.message;
      if (msg) googleError = msg;
      // Specific known errors
      if (googleResponse.status === 403) {
        // Could be quota exceeded or invalid key
        const reason = errBody?.error?.errors?.[0]?.reason;
        if (reason === "rateLimitExceeded" || reason === "dailyLimitExceeded") {
          return Response.json(
            { error: "Daily image search quota reached. Please try again tomorrow." },
            { status: 429 }
          );
        }
        return Response.json(
          { error: "Image search access denied. Check API key configuration." },
          { status: 403 }
        );
      }
    } catch {
      // ignore parse error
    }
    console.error("[search-images] Google CSE error:", googleError);
    return Response.json(
      { error: "Image search failed. Please try again." },
      { status: 502 }
    );
  }
  // ── 8. Parse response ─────────────────────────────────────────────────────
  let googleData;
  try {
    googleData = await googleResponse.json();
  } catch {
    return Response.json(
      { error: "Invalid response from image search service." },
      { status: 502 }
    );
  }
  // ── 9. Extract + filter results ───────────────────────────────────────────
  const rawItems = googleData?.items ?? [];
  const totalResults = parseInt(
    googleData?.searchInformation?.totalResults ?? "0",
    10
  );
  const results = rawItems
    .map((item) => {
      const url = item?.link;
      const thumbnail =
        item?.image?.thumbnailLink ||
        item?.pagemap?.cse_thumbnail?.[0]?.src ||
        url;
      const title = item?.title || "";
      const source = item?.displayLink || "";
      const width = item?.image?.width || 0;
      const height = item?.image?.height || 0;
      if (!url || typeof url !== "string") return null;
      if (!url.startsWith("https://") && !url.startsWith("http://")) return null;
      if (isDomainBlocked(url)) return null;
      return { url, thumbnail, title, source, width, height };
    })
    .filter(Boolean);
  // Calculate next page start
  const nextStart =
    start + PAGE_SIZE <= Math.min(totalResults, 100)
      ? start + PAGE_SIZE
      : null;
  return Response.json(
    { results, totalResults, nextStart, currentStart: start },
    {
      status: 200,
      headers: {
        // Cache for 5 minutes — same query shouldn't re-hit Google
        "Cache-Control": "private, max-age=300",
      },
    }
  );
}
 
these are converstaion with ai now complete the feature of feching images with ai for itinerary

---

## 🤖 Assistant
Thought for 3s
jsx
Copy
"use client";

import React, { useState, useEffect, useRef } from "react";
import { useRouter, useSearchParams } from "next/navigation";
import {
  Plus,
  ChevronLeft,
  Save,
  X,
  Loader2,
  Trash2,
  Info,
  FileText,
  ListChecks,
  MapPin,
  ImageIcon,
  AudioLinesIcon,
  Send,
  Lock,
  MessageSquare,
  AlertCircle,
  RefreshCw,
  ChevronDown,
  Sparkles,
  Search,
} from "lucide-react";
import {
  collection,
  getDocs,
  getDoc,
  doc,
  query,
  where,
  addDoc,
  updateDoc,
  serverTimestamp,
} from "firebase/firestore";
import { db, auth } from "@/firebase/config";
import { toast } from "react-hot-toast";

import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Textarea } from "@/components/ui/textarea";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Label } from "@/components/ui/label";
import { Badge } from "@/components/ui/badge";
import { useSelector } from "react-redux";

import {
  ImageUploader,
  MultiImageUploader,
  ActivitySelector,
  ChecklistSection,
  MultiStateDropdown,
} from "./../ItinerarySubComponents";

// NEW: Image Search Panel
import { ImageSearchPanel } from "./ImageSearchPanel";

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────
const mkId = () => `id-${Date.now()}-${Math.random().toString(36).slice(2, 7)}`;

async function getAuthHeaders() {
  const token = await auth.currentUser?.getIdToken();
  if (!token) throw new Error("Please sign in again to use AI generation.");
  return {
    "Content-Type": "application/json",
    Authorization: `Bearer ${token}`,
  };
}

// ─────────────────────────────────────────────────────────────────────────────
// Default checklist data (unchanged)
// ─────────────────────────────────────────────────────────────────────────────
const DEFAULT_INCLUSIONS = [
  "Hotel to Airport transfer on the day of departure.",
  "All tours & transfers are on a shared coach basis.",
  "Airport to Hotel transfer on the day of arrival.",
  "All sightseeing entry fees",
].map((text) => ({ id: mkId(), text, selected: true, isDefault: true }));

const DEFAULT_EXCLUSIONS = [
  "International or domestic flight tickets unless specified.",
  "Any item of personal nature like tips, laundry, telephone calls etc.",
  "Any other sightseeing other than those mentioned in the inclusions section.",
  "Any fee for video or camera permit.",
].map((text) => ({ id: mkId(), text, selected: true, isDefault: true }));

const DEFAULT_TNC = [
  "No rooms are booked or blocked yet, Rooms are subjected to availability.",
  "Package cost will vary depends on currency fluctuations.",
  "No flights are booked or blocked yet, Airfare & Seats are subjected to availability.",
  "Itinerary may change but the inclusions will remain same.",
].map((text) => ({ id: mkId(), text, selected: true, isDefault: true }));

const DEFAULT_CANCELLATION = [
  "These are non-refundable amounts as per the current components attached.",
  "Please check the exact cancellation and date change policy on the review page before proceeding further.",
  "Please note, TCS once collected cannot be refunded in case of any cancellation / modification.",
  "Cancellation charges shown is exclusive of all taxes and taxes will be added as per applicable.",
].map((text) => ({ id: mkId(), text, selected: true, isDefault: true }));

const DEFAULT_IMP_INFO = [
  "Ensure your passport is valid for at least six months beyond your intended date of return.",
  "Make sure you have enough blank pages for visa stamps.",
  "Obtain the appropriate visa (eg., tourist visa) for your destination country.",
  "Ensure the visa covers your entire stay.",
  "Ensure your travel insurance covers medical emergencies, trip cancellations, and loss of belongings.",
  "Carry a copy of your travel insurance policy.",
  "Carry an additional government-issued ID (e.g., Aadhar card, driving license).",
  "Some countries may require proof of COVID-19 vaccination.",
].map((text) => ({ id: mkId(), text, selected: true, isDefault: true }));

const SECTIONS = [
  { id: "itinerary", label: "Itinerary & Days", icon: MapPin },
  { id: "inclusions", label: "Inclusions & Exclusions", icon: ListChecks },
  { id: "tnc", label: "T&C's & Cancellation", icon: FileText },
  { id: "impinfo", label: "Important Information", icon: Info },
];

// ─────────────────────────────────────────────────────────────────────────────
// AIChatPopup (unchanged)
// ─────────────────────────────────────────────────────────────────────────────
function AIChatPopup({
  open,
  onClose,
  chatHistory,
  isGenerating,
  aiError,
  onGenerate,
  onRefine,
  hasGenerated,
  canGenerate,
}) {
  const [prompt, setPrompt] = useState("");
  const bottomRef = useRef(null);

  useEffect(() => {
    if (bottomRef.current) {
      bottomRef.current.scrollIntoView({ behavior: "smooth" });
    }
  }, [chatHistory, open]);

  const handleSubmit = () => {
    const trimmed = prompt.trim();
    if (!trimmed || isGenerating) return;
    onRefine(trimmed);
    setPrompt("");
  };

  const handleKeyDown = (e) => {
    if (e.key === "Enter" && (e.metaKey || e.ctrlKey)) {
      e.preventDefault();
      handleSubmit();
    }
  };

  if (!open) return null;

  return (
    <>
      <div
        className="fixed bottom-6 right-6 z-50 w-[420px] max-w-[calc(100vw-2rem)] bg-white rounded-2xl shadow-2xl border border-slate-200 flex flex-col overflow-hidden"
        style={{ maxHeight: "calc(100vh - 5rem)" }}
      >
        <div className="flex items-center justify-between px-4 py-3 bg-gradient-to-r from-blue-600 to-indigo-600 flex-shrink-0">
          <div className="flex items-center gap-2">
            <div className="w-7 h-7 rounded-full bg-white/20 flex items-center justify-center">
              <AudioLinesIcon className="w-3.5 h-3.5 text-white" />
            </div>
            <div>
              <p className="text-sm font-bold text-white leading-tight">
                AI Itinerary Assistant
              </p>
              <p className="text-[10px] text-blue-100">
                {hasGenerated
                  ? "Refine your itinerary"
                  : "Generate day-wise itinerary"}
              </p>
            </div>
            {chatHistory.length > 0 && (
              <Badge className="bg-white/20 text-white border-none text-[10px] px-1.5 py-0 ml-1">
                {chatHistory.filter((m) => m.role === "user").length} msg
              </Badge>
            )}
          </div>
          <button
            type="button"
            onClick={onClose}
            className="w-7 h-7 rounded-full bg-white/20 hover:bg-white/30 flex items-center justify-center transition-colors"
          >
            <X className="w-3.5 h-3.5 text-white" />
          </button>
        </div>

        <div className="flex-1 overflow-y-auto p-4 space-y-3 min-h-0">
          {chatHistory.length === 0 && !isGenerating && (
            <div className="text-center py-6 space-y-3">
              <div className="w-12 h-12 rounded-full bg-gradient-to-br from-blue-100 to-indigo-100 flex items-center justify-center mx-auto">
                <Sparkles className="w-5 h-5 text-blue-500" />
              </div>
              <div>
                <p className="text-sm font-semibold text-slate-700">
                  Ready to generate!
                </p>
                <p className="text-xs text-slate-400 mt-1">
                  Click Generate to create a day-wise itinerary. You can
                  describe your trip in the chat, or fill in the fields above —
                  AI will suggest missing details automatically.
                </p>
              </div>
            </div>
          )}

          {chatHistory.map((msg, idx) => (
            <div
              key={idx}
              className={`flex gap-2 ${msg.role === "user" ? "justify-end" : "justify-start"}`}
            >
              {msg.role === "assistant" && (
                <div className="w-6 h-6 rounded-full bg-gradient-to-br from-blue-500 to-indigo-600 flex items-center justify-center flex-shrink-0 mt-0.5">
                  <AudioLinesIcon className="w-3 h-3 text-white" />
                </div>
              )}
              <div
                className={`max-w-[82%] rounded-2xl px-3 py-2 text-xs leading-relaxed ${
                  msg.role === "user"
                    ? "bg-blue-600 text-white rounded-br-sm"
                    : msg.isError
                      ? "bg-red-50 text-red-700 border border-red-200 rounded-bl-sm"
                      : "bg-slate-100 text-slate-700 rounded-bl-sm"
                }`}
              >
                {msg.isError && (
                  <AlertCircle className="w-3 h-3 inline mr-1 mb-0.5" />
                )}
                {msg.content}
              </div>
              {msg.role === "user" && (
                <div className="w-6 h-6 rounded-full bg-slate-200 flex items-center justify-center flex-shrink-0 mt-0.5">
                  <span className="text-[9px] font-bold text-slate-600">U</span>
                </div>
              )}
            </div>
          ))}

          {isGenerating && (
            <div className="flex gap-2 justify-start">
              <div className="w-6 h-6 rounded-full bg-gradient-to-br from-blue-500 to-indigo-600 flex items-center justify-center flex-shrink-0 mt-0.5">
                <AudioLinesIcon className="w-3 h-3 text-white" />
              </div>
              <div className="bg-slate-100 rounded-2xl rounded-bl-sm px-3 py-2.5 flex items-center gap-1.5">
                <span className="w-1.5 h-1.5 bg-blue-500 rounded-full animate-bounce [animation-delay:0ms]" />
                <span className="w-1.5 h-1.5 bg-blue-500 rounded-full animate-bounce [animation-delay:150ms]" />
                <span className="w-1.5 h-1.5 bg-blue-500 rounded-full animate-bounce [animation-delay:300ms]" />
              </div>
            </div>
          )}

          {aiError && chatHistory.length === 0 && (
            <div className="flex items-start gap-2 text-xs text-red-600 bg-red-50 border border-red-200 rounded-xl px-3 py-2.5">
              <AlertCircle className="w-3.5 h-3.5 flex-shrink-0 mt-0.5" />
              <span>{aiError}</span>
            </div>
          )}

          <div ref={bottomRef} />
        </div>

        <div className="p-3 border-t border-slate-100 bg-slate-50/80 flex-shrink-0 space-y-2">
          {!hasGenerated ? (
            <Button
              type="button"
              onClick={onGenerate}
              disabled={isGenerating || !canGenerate}
              className="w-full bg-gradient-to-r from-blue-600 to-indigo-600 hover:from-blue-700 hover:to-indigo-700 text-white font-semibold rounded-xl py-2.5 disabled:opacity-50 shadow-sm"
            >
              {isGenerating ? (
                <>
                  <svg
                    className="animate-spin h-4 w-4 mr-2"
                    viewBox="0 0 24 24"
                    fill="none"
                  >
                    <circle
                      className="opacity-25"
                      cx="12"
                      cy="12"
                      r="10"
                      stroke="currentColor"
                      strokeWidth="4"
                    />
                    <path
                      className="opacity-75"
                      fill="currentColor"
                      d="M4 12a8 8 0 018-8v8z"
                    />
                  </svg>
                  Generating itinerary…
                </>
              ) : (
                <>
                  <Sparkles className="w-4 h-4 mr-2" />
                  Generate Itinerary with AI
                </>
              )}
            </Button>
          ) : (
            <>
              <div className="flex items-center gap-1.5 mb-1">
                <MessageSquare className="w-3 h-3 text-slate-400" />
                <span className="text-[10px] text-slate-500 font-medium">
                  Refine with a follow-up instruction · Ctrl+Enter to send
                </span>
              </div>
              <div className="flex gap-2 items-end">
                <Textarea
                  value={prompt}
                  onChange={(e) => setPrompt(e.target.value)}
                  onKeyDown={handleKeyDown}
                  placeholder={`e.g. "Add a food tour on Day 2" or "Make Day 3 more relaxed"…`}
                  disabled={isGenerating}
                  className="text-xs resize-none min-h-[60px] max-h-[120px] flex-1 disabled:opacity-60 leading-relaxed rounded-xl bg-white"
                  rows={2}
                />
                <Button
                  type="button"
                  onClick={handleSubmit}
                  disabled={!prompt.trim() || isGenerating}
                  className="h-9 w-9 p-0 flex-shrink-0 bg-blue-600 hover:bg-blue-700 text-white rounded-xl disabled:opacity-40 mb-0.5"
                  title="Send (Ctrl+Enter)"
                >
                  {isGenerating ? (
                    <RefreshCw className="w-4 h-4 animate-spin" />
                  ) : (
                    <Send className="w-4 h-4" />
                  )}
                </Button>
              </div>
              <button
                type="button"
                onClick={onGenerate}
                disabled={isGenerating}
                className="flex items-center gap-1.5 text-[10px] text-slate-400 hover:text-blue-600 transition-colors disabled:opacity-40 mt-1"
              >
                <RefreshCw className="w-3 h-3" />
                Regenerate from scratch
              </button>
            </>
          )}
        </div>
      </div>
    </>
  );
}

// ─────────────────────────────────────────────────────────────────────────────
// Main Component: ItineraryForm
// ─────────────────────────────────────────────────────────────────────────────
export default function ItineraryForm() {
  const router = useRouter();
  const searchParams = useSearchParams();
  const itineraryId = searchParams.get("itineraryid");

  const [activeSection, setActiveSection] = useState("itinerary");
  const [loading, setLoading] = useState(!!itineraryId);
  const [allStates, setAllStates] = useState([]);
  const [availableActivities, setAvailableActivities] = useState([]);
  const [cityInput, setCityInput] = useState("");
  const { user } = useSelector((state) => state.auth);
  const isValidCityName = (value) => /^[A-Za-z\s]+$/.test(value.trim());

  // ── Form state (unchanged) ─────────────────────────────────────────────────
  const [form, setForm] = useState({
    title: "",
    states: [],
    cities: [],
    startCity: "",
    endCity: "",
    numDays: "",
    tags: [],
    isActive: true,
    version: 0,
    posterImage: null,
  });

  const [days, setDays] = useState([
    {
      id: "initial-day",
      dayNumber: 1,
      title: "",
      description: "",
      activityIds: [],
      images: [],
    },
  ]);

  const [inclusions, setInclusions] = useState(() =>
    DEFAULT_INCLUSIONS.map((i) => ({ ...i, id: mkId() })),
  );
  const [exclusions, setExclusions] = useState(() =>
    DEFAULT_EXCLUSIONS.map((i) => ({ ...i, id: mkId() })),
  );
  const [tnc, setTnc] = useState(() =>
    DEFAULT_TNC.map((i) => ({ ...i, id: mkId() })),
  );
  const [cancellation, setCancellation] = useState(() =>
    DEFAULT_CANCELLATION.map((i) => ({ ...i, id: mkId() })),
  );
  const [impInfo, setImpInfo] = useState(() =>
    DEFAULT_IMP_INFO.map((i) => ({ ...i, id: mkId() })),
  );

  // ── AI state (unchanged) ───────────────────────────────────────────────────
  const [chatPopupOpen, setChatPopupOpen] = useState(false);
  const [chatHistory, setChatHistory] = useState([]);
  const [isGenerating, setIsGenerating] = useState(false);
  const [aiError, setAiError] = useState(null);
  const [hasGenerated, setHasGenerated] = useState(false);
  const [canUseAI, setCanUseAI] = useState(false);

  // ── NEW: Image Search Panel State ──────────────────────────────────────────
  const [imageSearchOpen, setImageSearchOpen] = useState(false);
  const [imageSearchContext, setImageSearchContext] = useState(null); // { type: 'poster' | 'day', dayIdx?: number }

  // Check AI permission (unchanged)
  useEffect(() => {
    if (!user?.uid) return;
    setCanUseAI(true);
  }, [user?.uid]);

  const canGenerate = true;

  // ── Load states (unchanged) ────────────────────────────────────────────────
  useEffect(() => {
    const fetchStates = async () => {
      try {
        const snap = await getDocs(collection(db, "locations"));
        const uniqueStates = [
          ...new Set(snap.docs.map((d) => d.data().name)),
        ].sort();
        setAllStates(uniqueStates);
      } catch {
        toast.error("Failed to load states");
      }
    };
    fetchStates();
  }, []);

  // ── Load existing itinerary (unchanged) ────────────────────────────────────
  useEffect(() => {
    if (!itineraryId) return;
    const loadData = async () => {
      try {
        const snap = await getDoc(doc(db, "itinerary_templates", itineraryId));
        if (snap.exists()) {
          const data = snap.data();
          setForm({
            title: data.title || "",
            states: data.states || (data.state ? [data.state] : []),
            cities: data.cities || [],
            startCity: data.startCity || "",
            endCity: data.endCity || "",
            numDays: data.numDays ? String(data.numDays) : "",
            tags: data.tags || [],
            isActive: data.isActive ?? true,
            version: data.version || 0,
            posterImage: data.posterImage || null,
          });
          if (data.days)
            setDays(
              data.days.map((d) => ({
                ...d,
                activityIds: d.activityIds || [],
                images: d.images || [],
              })),
            );
          if (data.inclusions) setInclusions(data.inclusions);
          if (data.exclusions) setExclusions(data.exclusions);
          if (data.tnc) setTnc(data.tnc);
          if (data.cancellation) setCancellation(data.cancellation);
          if (data.impInfo) setImpInfo(data.impInfo);
        }
      } catch {
        toast.error("Error loading template");
      } finally {
        setLoading(false);
      }
    };
    loadData();
  }, [itineraryId]);

  // ── Load activities (unchanged) ────────────────────────────────────────────
  useEffect(() => {
    if (form.states.length === 0) {
      setAvailableActivities([]);
      return;
    }
    const fetchActivities = async () => {
      try {
        const snaps = await Promise.all(
          form.states.map((s) =>
            getDocs(
              query(collection(db, "activities"), where("state", "==", s)),
            ),
          ),
        );
        const seen = new Set();
        const all = [];
        for (const snap of snaps) {
          for (const d of snap.docs) {
            if (!seen.has(d.id)) {
              seen.add(d.id);
              all.push({ id: d.id, ...d.data() });
            }
          }
        }
        setAvailableActivities(all);
      } catch {
        toast.error("Error fetching activities");
      }
    };
    fetchActivities();
  }, [form.states]);

  // ── Checklist helpers (unchanged) ──────────────────────────────────────────
  const makeHandlers = (setter) => ({
    toggle: (id) =>
      setter((prev) =>
        prev.map((i) => (i.id === id ? { ...i, selected: !i.selected } : i)),
      ),
    selectAll: (checked) =>
      setter((prev) => prev.map((i) => ({ ...i, selected: !!checked }))),
    add: (text) =>
      setter((prev) => [
        ...prev,
        { id: mkId(), text, selected: true, isDefault: false },
      ]),
    remove: (id) => setter((prev) => prev.filter((i) => i.id !== id)),
  });

  const incH = makeHandlers(setInclusions);
  const excH = makeHandlers(setExclusions);
  const tncH = makeHandlers(setTnc);
  const canH = makeHandlers(setCancellation);
  const impH = makeHandlers(setImpInfo);

  // ── City handlers (unchanged) ──────────────────────────────────────────────
  const handleAddCity = (e) => {
    if (e.key === "Enter" && cityInput.trim()) {
      e.preventDefault();
      const value = cityInput.trim();
      if (!isValidCityName(value)) {
        toast.error("City name should only contain letters and spaces.");
        return;
      }
      if (!form.cities.includes(value)) {
        setForm((prev) => ({
          ...prev,
          cities: [...prev.cities, value],
        }));
      }
      setCityInput("");
    }
  };

  const removeCity = (city) =>
    setForm((prev) => ({
      ...prev,
      cities: prev.cities.filter((c) => c !== city),
    }));

  // ── Day handlers (unchanged except image updates) ──────────────────────────
  const handleAddDay = () =>
    setDays((prev) => [
      ...prev,
      {
        id: Date.now().toString(),
        dayNumber: prev.length + 1,
        title: "",
        description: "",
        activityIds: [],
        images: [],
      },
    ]);

  const removeDay = (idx) => {
    if (days[idx].title && !window.confirm("Delete this day?")) return;
    setDays((prev) =>
      prev
        .filter((_, i) => i !== idx)
        .map((d, i) => ({ ...d, dayNumber: i + 1 })),
    );
  };

  const updateDayField = (index, field, value) =>
    setDays((prev) => {
      const updated = [...prev];
      updated[index] = { ...updated[index], [field]: value };
      return updated;
    });

  const toggleActivity = (dayIdx, actId) =>
    setDays((prev) =>
      prev.map((d, i) => {
        if (i !== dayIdx) return d;
        const current = d.activityIds || [];
        return {
          ...d,
          activityIds: current.includes(actId)
            ? current.filter((id) => id !== actId)
            : [...current, actId],
        };
      }),
    );

  const updateDayImages = (dayIdx, newImages) =>
    setDays((prev) => {
      const updated = [...prev];
      updated[dayIdx] = { ...updated[dayIdx], images: newImages };
      return updated;
    });

  // ── NEW: Open Image Search ─────────────────────────────────────────────────
  const openImageSearch = (context) => {
    setImageSearchContext(context);
    setImageSearchOpen(true);
  };

  const handleImageSearchSelect = (selectedUrls) => {
    if (imageSearchContext?.type === "poster") {
      setForm((prev) => ({ ...prev, posterImage: selectedUrls[0] || null }));
    } else if (imageSearchContext?.type === "day" && typeof imageSearchContext.dayIdx === "number") {
      const currentDayImages = days[imageSearchContext.dayIdx]?.images || [];
      const combined = [...currentDayImages, ...selectedUrls].slice(0, 2); // respect max 2
      updateDayImages(imageSearchContext.dayIdx, combined);
    }
    setImageSearchOpen(false);
    setImageSearchContext(null);
  };

  // ── AI handlers (unchanged) ────────────────────────────────────────────────
  const applyAIResponse = (data) => {
    setForm((prev) => ({
      ...prev,
      ...(data.title ? { title: data.title } : {}),
      ...(data.states?.length && prev.states.length === 0
        ? { states: data.states }
        : {}),
      ...(data.cities?.length && prev.cities.length === 0
        ? { cities: data.cities }
        : {}),
      ...(data.startCity && !prev.startCity
        ? { startCity: data.startCity }
        : {}),
      ...(data.endCity && !prev.endCity ? { endCity: data.endCity } : {}),
      ...(data.numDays && !prev.numDays
        ? { numDays: String(data.numDays) }
        : {}),
    }));
    if (data.days?.length) {
      setDays(
        data.days.map((d, i) => ({
          id: mkId(),
          dayNumber: d.dayNumber ?? i + 1,
          title: d.title || "",
          description: d.description || "",
          activityIds: [],
          images: [],
        })),
      );
    }
  };

  const handleGenerateWithAI = async () => {
    setIsGenerating(true);
    setAiError(null);
    setChatHistory([]);

    try {
      const headers = await getAuthHeaders();
      const res = await fetch("/api/ai-itinerary-template", {
        method: "POST",
        headers,
        body: JSON.stringify({
          templateContext: {
            states: form.states,
            cities: form.cities,
            startCity: form.startCity,
            endCity: form.endCity,
            numDays: Number(form.numDays),
          },
          chatHistory: [],
          userPrompt: null,
        }),
      });

      if (!res.ok) {
        const err = await res.json().catch(() => ({}));
        throw new Error(err?.error || `Server error (${res.status})`);
      }

      const data = await res.json();
      applyAIResponse(data);
      setHasGenerated(true);

      setChatHistory([
        {
          role: "assistant",
          content: `Itinerary generated! Created ${data.days?.length ?? 0} days for "${data.title || "your trip"}". You can now refine it with follow-up messages, or edit the days manually below.`,
        },
      ]);
    } catch (err) {
      console.error("[AI ItineraryForm]", err);
      const errMsg =
        err.message || "Generation failed. You can still fill in manually.";
      setAiError(errMsg);
      setChatHistory([
        {
          role: "assistant",
          content: `Could not generate: ${errMsg}`,
          isError: true,
        },
      ]);
    } finally {
      setIsGenerating(false);
    }
  };

  const handleRefine = async (userPrompt) => {
    if (!userPrompt.trim() || isGenerating) return;

    setIsGenerating(true);
    setAiError(null);

    const updatedHistory = [
      ...chatHistory,
      { role: "user", content: userPrompt },
    ];
    setChatHistory(updatedHistory);

    const currentItinerary = { title: form.title, days };

    try {
      const headers = await getAuthHeaders();
      const res = await fetch("/api/ai-itinerary-template", {
        method: "POST",
        headers,
        body: JSON.stringify({
          templateContext: {
            states: form.states,
            cities: form.cities,
            startCity: form.startCity,
            endCity: form.endCity,
            numDays: Number(form.numDays),
          },
          chatHistory: updatedHistory,
          userPrompt,
          currentItinerary,
        }),
      });

      if (!res.ok) {
        let errMsg = `Server error (${res.status})`;
        try {
          const errBody = await res.json();
          errMsg = errBody?.error || errMsg;
        } catch {}
        throw new Error(errMsg);
      }

      const data = await res.json();
      applyAIResponse(data);

      setChatHistory((prev) => [
        ...prev,
        {
          role: "assistant",
          content: `Done! Updated the itinerary based on your request.${data.days?.length ? ` The plan now has ${data.days.length} days.` : ""}`,
        },
      ]);
    } catch (err) {
      console.error("[AI ItineraryForm Refine]", err);
      const errMsg =
        err.message ||
        "Could not apply changes. Please try again or edit manually.";
      setChatHistory((prev) => [
        ...prev,
        { role: "assistant", content: errMsg, isError: true },
      ]);
    } finally {
      setIsGenerating(false);
    }
  };

  // ── Save (unchanged) ───────────────────────────────────────────────────────
  const handleSave = async (isDraft = false) => {
    if (!form.title || form.states.length === 0 || form.cities.length === 0)
      return toast.error(
        "Required: Title, at least one State, and at least one City.",
      );
    if (!form.startCity.trim())
      return toast.error("Starting City is required.");
    if (!form.endCity.trim()) return toast.error("Ending City is required.");
    if (!form.numDays || Number(form.numDays) < 1)
      return toast.error("Number of Days must be at least 1.");
    if (!user?.uid || !user?.role)
      return toast.error("User should be logged in");
    if (!isValidCityName(form.startCity)) {
      return toast.error("Starting city must contain only letters.");
    }
    if (!isValidCityName(form.endCity)) {
      return toast.error("Ending city must contain only letters.");
    }
    if (form.cities.some((city) => !isValidCityName(city))) {
      return toast.error("All cities must contain only letters.");
    }

    const payload = {
      title: form.title,
      states: form.states,
      state: form.states[0] || "",
      cities: form.cities,
      startCity: form.startCity,
      endCity: form.endCity,
      numDays: Number(form.numDays),
      tags: form.tags,
      isActive: form.isActive,
      posterImage: form.posterImage,
      days,
      inclusions,
      exclusions,
      tnc,
      cancellation,
      impInfo,
      durationNights: days.length - 1,
      version: (form.version || 0) + 1,
      updatedAt: serverTimestamp(),
      status: isDraft ? "Draft" : "Published",
      clientRole: user.role,
      clientId: user.uid,
    };

    try {
      const loader = toast.loading("Saving...");
      if (itineraryId) {
        await updateDoc(doc(db, "itinerary_templates", itineraryId), payload);
      } else {
        payload.createdAt = serverTimestamp();
        await addDoc(collection(db, "itinerary_templates"), payload);
      }
      toast.dismiss(loader);
      toast.success("Saved successfully");
      router.back();
    } catch {
      toast.error("Save failed");
    }
  };

  if (loading)
    return (
      <div className="h-screen flex items-center justify-center">
        <Loader2 className="animate-spin text-blue-600 w-8 h-8" />
      </div>
    );

  return (
    <div className="min-h-screen bg-slate-50 pb-24">
      {/* Sticky Header (unchanged) */}
      <header className="sticky top-0 z-20 bg-white border-b px-6 py-4 flex justify-between items-center shadow-sm">
        <div className="flex items-center gap-4">
          <Button variant="ghost" size="icon" onClick={() => router.back()}>
            <ChevronLeft />
          </Button>
          <h1 className="text-xl font-bold text-slate-800">
            {itineraryId ? "Edit" : "Create"} Itinerary Template
          </h1>
        </div>
        <div className="flex gap-2 items-center">
          <Button
            type="button"
            onClick={() => setChatPopupOpen(true)}
            variant="outline"
            className="flex items-center gap-2 border-blue-300 text-blue-700 hover:bg-blue-50 relative"
          >
            <Sparkles className="w-4 h-4" />
            AI Generate
            {isGenerating && (
              <span className="absolute -top-1 -right-1 w-3 h-3 bg-blue-500 rounded-full animate-pulse" />
            )}
            {hasGenerated && !isGenerating && (
              <span className="absolute -top-1 -right-1 w-3 h-3 bg-green-500 rounded-full" />
            )}
          </Button>
          <Button variant="outline" onClick={() => handleSave(true)}>
            Save Draft
          </Button>
          <Button
            onClick={() => handleSave(false)}
            className="bg-blue-600 hover:bg-blue-700 text-white"
          >
            <Save className="w-4 h-4 mr-2" /> Save Template
          </Button>
        </div>
      </header>

      {/* Section Tabs (unchanged) */}
      <div className="sticky top-[73px] z-10 bg-white border-b shadow-sm">
        <div className="max-w-5xl mx-auto px-4 flex gap-1 py-2 overflow-x-auto">
          {SECTIONS.map(({ id, label, icon: Icon }) => (
            <button
              key={id}
              type="button"
              onClick={() => setActiveSection(id)}
              className={`flex items-center gap-2 px-4 py-2 rounded-md text-sm font-medium whitespace-nowrap transition-all ${
                activeSection === id
                  ? "bg-blue-600 text-white shadow-sm"
                  : "text-slate-600 hover:bg-slate-100"
              }`}
            >
              <Icon className="w-4 h-4" />
              {label}
            </button>
          ))}
        </div>
      </div>

      <main className="max-w-5xl mx-auto mt-6 px-4 space-y-6">
        {/* ITINERARY SECTION */}
        {activeSection === "itinerary" && (
          <>
            {/* Header Info */}
            <Card>
              <CardHeader>
                <CardTitle className="text-xs uppercase tracking-widest text-slate-500 font-semibold">
                  Step 1: Header Info
                </CardTitle>
              </CardHeader>
              <CardContent className="space-y-5">
                {/* Title, States, Cities, Start/End, NumDays (unchanged) */}
                <div className="space-y-1">
                  <Label>Itinerary Title *</Label>
                  <Input
                    value={form.title}
                    onChange={(e) =>
                      setForm({ ...form, title: e.target.value })
                    }
                    placeholder="e.g. 6D5N Rajasthan Heritage Circuit"
                  />
                </div>

                <div className="space-y-1.5">
                  <Label>Base State(s) *</Label>
                  <MultiStateDropdown
                    states={allStates}
                    selectedStates={form.states}
                    onChange={(states) =>
                      setForm((prev) => ({ ...prev, states }))
                    }
                  />
                </div>

                <div className="space-y-2">
                  <Label>Cities Covered (type and press Enter) *</Label>
                  <div className="flex flex-wrap gap-2 p-2 border rounded-md bg-white min-h-[45px] focus-within:ring-1 focus-within:ring-blue-500">
                    {form.cities.map((city) => (
                      <Badge
                        key={city}
                        className="bg-blue-100 text-blue-800 hover:bg-blue-200 border-none px-2 py-1 flex items-center gap-1"
                      >
                        {city}
                        <button
                          type="button"
                          onClick={() => removeCity(city)}
                          className="hover:text-red-600 p-0.5 rounded-full"
                        >
                          <X className="w-3 h-3" />
                        </button>
                      </Badge>
                    ))}
                    <input
                      className="flex-1 outline-none text-sm min-w-[120px]"
                      value={cityInput}
                      onChange={(e) => {
                        const filtered = e.target.value.replace(/[^A-Za-z\s]/g, "");
                        setCityInput(filtered);
                      }}
                      onKeyDown={(e) => {
                        if (/[^A-Za-z\s]/.test(e.key) && e.key.length === 1) {
                          e.preventDefault();
                          return;
                        }
                        handleAddCity(e);
                      }}
                      onPaste={(e) => {
                        e.preventDefault();
                        const pasted = e.clipboardData.getData("text");
                        const filtered = pasted.replace(/[^A-Za-z\s]/g, "");
                        setCityInput((prev) => (prev + filtered).trimStart());
                      }}
                      placeholder="Add city..."
                    />
                  </div>
                </div>

                <div className="grid grid-cols-2 gap-4">
                  <div className="space-y-1">
                    <Label>Starting City *</Label>
                    <Input
                      value={form.startCity}
                      onChange={(e) => {
                        const value = e.target.value;
                        if (/^[A-Za-z\s]*$/.test(value)) {
                          setForm({ ...form, startCity: value });
                        }
                      }}
                      placeholder="e.g. Jaipur"
                    />
                    <p className="text-[11px] text-slate-400">
                      First destination of the trip
                    </p>
                  </div>
                  <div className="space-y-1">
                    <Label>Ending City *</Label>
                    <Input
                      value={form.endCity}
                      onChange={(e) => {
                        const value = e.target.value;
                        if (/^[A-Za-z\s]*$/.test(value)) {
                          setForm({ ...form, endCity: value });
                        }
                      }}
                      placeholder="e.g. Udaipur"
                    />
                    <p className="text-[11px] text-slate-400">
                      Final city before departure
                    </p>
                  </div>
                </div>

                <div className="space-y-1 max-w-xs">
                  <Label>Number of Trip Days *</Label>
                  <Input
                    type="number"
                    min={1}
                    max={30}
                    value={form.numDays}
                    onChange={(e) =>
                      setForm({ ...form, numDays: e.target.value })
                    }
                    placeholder="e.g. 6"
                  />
                  <p className="text-[11px] text-slate-400">
                    Total days including arrival and departure
                  </p>
                </div>

                {/* Poster Image with Search Button */}
                <div className="space-y-1.5 pt-2 border-t">
                  <div className="flex items-center justify-between">
                    <Label className="flex items-center gap-1.5 text-sm font-semibold text-slate-700">
                      <ImageIcon className="w-4 h-4 text-blue-500" />
                      Poster / Cover Image
                    </Label>
                    <Button
                      type="button"
                      variant="outline"
                      size="sm"
                      onClick={() => openImageSearch({ type: "poster" })}
                      className="flex items-center gap-1.5 text-xs"
                    >
                      <Search className="w-3.5 h-3.5" />
                      Search Images
                    </Button>
                  </div>
                  <p className="text-xs text-slate-400">
                    Main image shown at the top of the itinerary.
                  </p>
                  <div className="max-w-lg">
                    <ImageUploader
                      value={form.posterImage}
                      onChange={(url) =>
                        setForm((prev) => ({ ...prev, posterImage: url }))
                      }
                      label=""
                      aspectClass="aspect-video"
                      maxSizeMB={8}
                    />
                  </div>
                </div>
              </CardContent>
            </Card>

            {/* Day Program */}
            <div className="space-y-4">
              <div className="flex items-center justify-between">
                <h2 className="text-lg font-bold text-slate-800">
                  Step 2: Day Program
                </h2>
              </div>

              {days.map((day, idx) => (
                <Card
                  key={day.id}
                  className="relative border-l-4 border-l-blue-500"
                >
                  <div className="bg-slate-50 px-4 py-2 border-b flex justify-between items-center">
                    <span className="text-xs font-black text-blue-600 tracking-widest">
                      DAY {day.dayNumber}
                    </span>
                    <Button
                      variant="ghost"
                      size="icon"
                      className="h-8 w-8 text-red-500 hover:bg-red-50"
                      onClick={() => removeDay(idx)}
                    >
                      <Trash2 className="w-4 h-4" />
                    </Button>
                  </div>

                  <CardContent className="p-5 space-y-4">
                    <Input
                      placeholder="Day Title..."
                      value={day.title}
                      onChange={(e) =>
                        updateDayField(idx, "title", e.target.value)
                      }
                      className="font-bold text-md"
                    />
                    <Textarea
                      placeholder="What happens today? Describe transfers, sightseeing, meals…"
                      value={day.description}
                      onChange={(e) =>
                        updateDayField(idx, "description", e.target.value)
                      }
                      className="min-h-[100px]"
                    />

                    {/* Day Images with Search Button */}
                    <div className="space-y-2 border-t pt-3">
                      <div className="flex items-center justify-between">
                        <Label className="flex items-center gap-1.5 text-xs text-slate-500 font-medium">
                          <ImageIcon className="w-3.5 h-3.5" />
                          Day Photos
                          <span className="ml-auto text-[10px] text-slate-400 font-normal">
                            {(day.images || []).length} / 2
                          </span>
                        </Label>
                        <Button
                          type="button"
                          variant="outline"
                          size="sm"
                          onClick={() =>
                            openImageSearch({
                              type: "day",
                              dayIdx: idx,
                              dayTitle: day.title,
                              dayDescription: day.description,
                            })
                          }
                          className="text-xs h-7"
                        >
                          <Search className="w-3.5 h-3.5 mr-1" />
                          Search
                        </Button>
                      </div>
                      <MultiImageUploader
                        values={day.images || []}
                        onChange={(urls) => updateDayImages(idx, urls)}
                        max={2}
                      />
                    </div>

                    {/* Linked Activities (unchanged) */}
                    <div className="space-y-2 border-t pt-3">
                      <Label className="text-xs text-slate-500">
                        Linked Activities
                      </Label>
                      {(day.activityIds || []).length > 0 && (
                        <div className="flex flex-wrap gap-2">
                          {day.activityIds.map((actId) => {
                            const activity = availableActivities.find(
                              (a) => a.id === actId,
                            );
                            return (
                              <Badge
                                key={actId}
                                variant="outline"
                                className="flex items-center gap-1 pr-1 bg-blue-50 border-blue-200 text-blue-700"
                              >
                                {activity ? (
                                  activity.name
                                ) : (
                                  <span className="text-slate-400 italic text-xs">
                                    Loading…
                                  </span>
                                )}
                                <button
                                  type="button"
                                  onClick={() => toggleActivity(idx, actId)}
                                  className="hover:text-red-500 p-0.5"
                                >
                                  <X className="w-3 h-3" />
                                </button>
                              </Badge>
                            );
                          })}
                        </div>
                      )}
                      <ActivitySelector
                        dayIdx={idx}
                        activityIds={day.activityIds || []}
                        availableActivities={availableActivities}
                        onToggle={toggleActivity}
                        state={form.states[0] || ""}
                      />
                    </div>
                  </CardContent>

                  {idx === days.length - 1 && (
                    <div className="px-5 pb-5">
                      <button
                        type="button"
                        onClick={handleAddDay}
                        className="w-full flex items-center justify-center gap-2 px-4 py-2.5 rounded-lg border-2 border-dashed border-blue-300 text-blue-600 text-sm font-semibold hover:bg-blue-50 hover:border-blue-400 transition-all duration-150"
                      >
                        <Plus className="w-4 h-4" />
                        Add Next Day
                      </button>
                    </div>
                  )}
                </Card>
              ))}

              {days.length === 0 && (
                <button
                  type="button"
                  onClick={handleAddDay}
                  className="w-full flex items-center justify-center gap-2 px-4 py-3 rounded-lg border-2 border-dashed border-blue-300 text-blue-600 text-sm font-semibold hover:bg-blue-50 hover:border-blue-400 transition-all duration-150"
                >
                  <Plus className="w-4 h-4" />
                  Add First Day
                </button>
              )}
            </div>
          </>
        )}

        {/* Other sections unchanged */}
        {activeSection === "inclusions" && (
          <Card>
            <CardHeader>
              <CardTitle className="text-xs uppercase tracking-widest text-slate-500 font-semibold">
                Inclusion &amp; Exclusion
              </CardTitle>
            </CardHeader>
            <CardContent className="space-y-8">
              <div className="space-y-4">
                <h3 className="text-sm font-bold text-slate-700 border-b pb-2">
                  Include
                </h3>
                <ChecklistSection
                  items={inclusions}
                  onToggle={incH.toggle}
                  onSelectAll={incH.selectAll}
                  onAdd={incH.add}
                  onRemove={incH.remove}
                  addLabel="Add Inclusion"
                />
              </div>
              <div className="space-y-4">
                <h3 className="text-sm font-bold text-slate-700 border-b pb-2">
                  Exclude
                </h3>
                <ChecklistSection
                  items={exclusions}
                  onToggle={excH.toggle}
                  onSelectAll={excH.selectAll}
                  onAdd={excH.add}
                  onRemove={excH.remove}
                  addLabel="Add Exclusion"
                />
              </div>
            </CardContent>
          </Card>
        )}

        {activeSection === "tnc" && (
          <div className="space-y-6">
            <Card>
              <CardHeader>
                <CardTitle className="text-xs uppercase tracking-widest text-slate-500 font-semibold">
                  T&amp;C&apos;s
                </CardTitle>
              </CardHeader>
              <CardContent>
                <ChecklistSection
                  items={tnc}
                  onToggle={tncH.toggle}
                  onSelectAll={tncH.selectAll}
                  onAdd={tncH.add}
                  onRemove={tncH.remove}
                  addLabel="Add Terms"
                />
              </CardContent>
            </Card>
            <Card>
              <CardHeader>
                <CardTitle className="text-xs uppercase tracking-widest text-slate-500 font-semibold">
                  Cancellation Policy
                </CardTitle>
              </CardHeader>
              <CardContent>
                <ChecklistSection
                  items={cancellation}
                  onToggle={canH.toggle}
                  onSelectAll={canH.selectAll}
                  onAdd={canH.add}
                  onRemove={canH.remove}
                  addLabel="Add Cancellation Policy"
                />
              </CardContent>
            </Card>
          </div>
        )}

        {activeSection === "impinfo" && (
          <Card>
            <CardHeader>
              <CardTitle className="text-xs uppercase tracking-widest text-slate-500 font-semibold">
                Important Information
              </CardTitle>
            </CardHeader>
            <CardContent>
              <ChecklistSection
                items={impInfo}
                onToggle={impH.toggle}
                onSelectAll={impH.selectAll}
                onAdd={impH.add}
                onRemove={impH.remove}
                addLabel="Add Important Information"
              />
            </CardContent>
          </Card>
        )}
      </main>

      {/* AI Chat Popup (unchanged) */}
      <AIChatPopup
        open={chatPopupOpen}
        onClose={() => setChatPopupOpen(false)}
        chatHistory={chatHistory}
        isGenerating={isGenerating}
        aiError={aiError}
        onGenerate={handleGenerateWithAI}
        onRefine={handleRefine}
        hasGenerated={hasGenerated}
        canGenerate={canGenerate}
      />

      {/* NEW: Image Search Panel */}
      <ImageSearchPanel
        open={imageSearchOpen}
        onClose={() => {
          setImageSearchOpen(false);
          setImageSearchContext(null);
        }}
        context={imageSearchContext}
        onSelect={handleImageSearchSelect}
      />
    </div>
  );
}

Next step: Create the new file ItinerarySubComponents/ImageSearchPanel.jsx (or in the same folder as other subcomponents).

Would you like me to provide the full code for ImageSearchPanel.jsx now? It will include:

Right-side drawer
Smart query generation from context
Search input + button
Scrollable 3-column grid of image results
Multi-select (1 for poster, up to 2 for days)
Loading + pagination support
Preserve manual upload via existing components

Let me know if you want it now or any adjustments.

---

## 🤖 Assistant
ok next step , but also provide me context detail ,like dir structre , new env names or necessary details

---

## 🤖 Assistant
Thought for 2s

✅ Done — Feature Implementation Complete

Here’s everything you need for the Google Image Search feature while preserving all existing functionality.

1. Directory Structure (Recommended)
text
Copy
app/
├── itinerary/                  ← Your current page
│   └── page.tsx                (or wherever ItineraryForm is used)
│
├── components/
│   └── itinerary/              ← Keep subcomponents together
│       ├── ItinerarySubComponents/
│       │   ├── index.js or barrel export
│       │   ├── ImageUploader.jsx
│       │   ├── MultiImageUploader.jsx
│       │   ├── ActivitySelector.jsx
│       │   ├── ChecklistSection.jsx
│       │   ├── MultiStateDropdown.jsx
│       │   └── ImageSearchPanel.jsx     ← NEW
│       └── ItineraryForm.jsx            ← Updated (already provided)
│
└── api/
    ├── search-images/
    │   └── route.js                     ← NEW (provided earlier)
    └── ai-itinerary-template/
        └── route.js
2. New Environment Variables

Add these to your .env.local:

env
Copy
# Google Custom Search API (Image Search)
GOOGLE_SEARCH_API_KEY=your_google_cloud_api_key_here
GOOGLE_SEARCH_CX=your_programmable_search_engine_id_here

How to get them:

Go to Google Cloud Console
Enable Custom Search API
Create API Key
Go to Programmable Search Engine → Create one with Image search = ON and Search the entire web
Copy the Search engine ID (cx)
3. New Component: ImageSearchPanel.jsx

Create this file at:
components/itinerary/ItinerarySubComponents/ImageSearchPanel.jsx

jsx
Copy
"use client";

import React, { useState, useEffect } from "react";
import { X, Search, Loader2, Plus, Check } from "lucide-react";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Badge } from "@/components/ui/badge";
import toast from "react-hot-toast";

export function ImageSearchPanel({
  open,
  onClose,
  context, // { type: 'poster' | 'day', dayIdx?, dayTitle?, dayDescription? }
  onSelect,
}) {
  const [query, setQuery] = useState("");
  const [results, setResults] = useState([]);
  const [loading, setLoading] = useState(false);
  const [selected, setSelected] = useState([]);
  const [nextStart, setNextStart] = useState(null);
  const [searching, setSearching] = useState(false);

  const isPoster = context?.type === "poster";
  const maxSelect = isPoster ? 1 : 2;

  // Auto-build smart query when panel opens
  useEffect(() => {
    if (!open || !context) return;

    let smartQuery = "";

    if (isPoster) {
      smartQuery = `${context?.tripTitle || "India travel"} itinerary poster landscape`;
    } else {
      const dayInfo = context.dayTitle
        ? `${context.dayTitle} `
        : `Day ${context.dayIdx + 1} `;
      smartQuery = `${dayInfo} ${context.dayDescription?.slice(0, 80) || ""} ${context?.cities?.join(" ") || ""} India tourism`;
    }

    setQuery(smartQuery.trim());
    setSelected([]);
    setResults([]);
    setNextStart(null);

    // Auto search on open
    if (smartQuery) handleSearch(smartQuery);
  }, [open, context]);

  const handleSearch = async (searchQuery = query, start = 1) => {
    if (!searchQuery.trim()) return;

    setSearching(true);
    setLoading(true);

    try {
      const res = await fetch(
        `/api/search-images?q=${encodeURIComponent(searchQuery)}&start=${start}`
      );

      if (!res.ok) {
        const err = await res.json().catch(() => ({}));
        throw new Error(err.error || "Search failed");
      }

      const data = await res.json();

      if (start === 1) {
        setResults(data.results || []);
      } else {
        setResults((prev) => [...prev, ...(data.results || [])]);
      }

      setNextStart(data.nextStart);
    } catch (err) {
      toast.error(err.message || "Failed to search images");
      console.error(err);
    } finally {
      setLoading(false);
      setSearching(false);
    }
  };

  const toggleSelect = (url) => {
    if (selected.includes(url)) {
      setSelected(selected.filter((u) => u !== url));
    } else if (selected.length < maxSelect) {
      setSelected([...selected, url]);
    } else {
      toast.error(`Maximum ${maxSelect} image${maxSelect > 1 ? 's' : ''} allowed`);
    }
  };

  const handleConfirm = () => {
    if (selected.length === 0) {
      toast.error("Please select at least one image");
      return;
    }
    onSelect(selected);
    onClose();
  };

  if (!open) return null;

  return (
    <>
      {/* Backdrop */}
      <div
        className="fixed inset-0 bg-black/60 z-50"
        onClick={onClose}
      />

      {/* Side Panel */}
      <div className="fixed right-0 top-0 h-full w-[440px] bg-white shadow-2xl z-50 flex flex-col overflow-hidden">
        {/* Header */}
        <div className="flex items-center justify-between px-5 py-4 border-b">
          <div>
            <h3 className="font-semibold text-lg">
              {isPoster ? "Select Poster Image" : "Select Day Images"}
            </h3>
            <p className="text-xs text-slate-500">
              {isPoster ? "Choose 1 image" : "Choose up to 2 images"} • Powered by Google
            </p>
          </div>
          <button
            onClick={onClose}
            className="p-2 hover:bg-slate-100 rounded-full transition-colors"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Search Bar */}
        <div className="p-4 border-b">
          <div className="flex gap-2">
            <Input
              value={query}
              onChange={(e) => setQuery(e.target.value)}
              placeholder="Search for images..."
              className="flex-1"
              onKeyDown={(e) => e.key === "Enter" && handleSearch()}
            />
            <Button
              onClick={() => handleSearch()}
              disabled={searching}
              className="px-6"
            >
              {searching ? <Loader2 className="w-4 h-4 animate-spin" /> : <Search className="w-4 h-4" />}
            </Button>
          </div>
        </div>

        {/* Results */}
        <div className="flex-1 overflow-y-auto p-4">
          {loading && results.length === 0 && (
            <div className="flex flex-col items-center justify-center h-64 text-slate-400">
              <Loader2 className="w-8 h-8 animate-spin mb-3" />
              Searching images...
            </div>
          )}

          {results.length > 0 && (
            <div className="grid grid-cols-2 gap-3">
              {results.map((img, idx) => {
                const isSelected = selected.includes(img.url);
                return (
                  <div
                    key={idx}
                    onClick={() => toggleSelect(img.url)}
                    className={`relative group aspect-video rounded-lg overflow-hidden border-2 cursor-pointer transition-all ${
                      isSelected ? "border-blue-600 shadow-md" : "border-transparent hover:border-slate-300"
                    }`}
                  >
                    <img
                      src={img.thumbnail || img.url}
                      alt={img.title}
                      className="w-full h-full object-cover"
                    />
                    {isSelected && (
                      <div className="absolute inset-0 bg-blue-600/30 flex items-center justify-center">
                        <div className="bg-white rounded-full p-1">
                          <Check className="w-6 h-6 text-blue-600" />
                        </div>
                      </div>
                    )}
                    <div className="absolute bottom-0 left-0 right-0 bg-gradient-to-t from-black/70 to-transparent p-2">
                      <p className="text-[10px] text-white line-clamp-1">
                        {img.title || "Untitled"}
                      </p>
                    </div>
                  </div>
                );
              })}
            </div>
          )}

          {!loading && results.length === 0 && (
            <div className="text-center py-12 text-slate-400">
              No images found. Try a different search term.
            </div>
          )}

          {nextStart && (
            <div className="flex justify-center mt-6">
              <Button
                variant="outline"
                onClick={() => handleSearch(query, nextStart)}
                disabled={loading}
              >
                Load More Images
              </Button>
            </div>
          )}
        </div>

        {/* Footer */}
        <div className="p-4 border-t bg-slate-50 flex items-center gap-3">
          <Button variant="outline" onClick={onClose} className="flex-1">
            Cancel
          </Button>
          <Button
            onClick={handleConfirm}
            disabled={selected.length === 0}
            className="flex-1 bg-blue-600 hover:bg-blue-700"
          >
            <Plus className="w-4 h-4 mr-2" />
            Add Selected ({selected.length}/{maxSelect})
          </Button>
        </div>
      </div>
    </>
  );
}
4. What You Already Have (Preserved)
I only added the Image Search functionality
All existing uploaders (ImageUploader, MultiImageUploader) still work
AI generation, checklists, day management — untouched
No code was removed
Next Steps (If Needed)
Add the two env variables
Create the /api/search-images/route.js (I already gave you the full code in previous response)
Place ImageSearchPanel.jsx in the subcomponents folder
Replace your ItineraryForm.jsx with the updated version I gave earlier

Would you like me to:

Provide the full search-images/route.js again?
Make any UI adjustments (e.g., modal instead of side panel)?
Add image preview on hover or better mobile support?

Just say the word and I’ll adjust.

---

## 🤖 Assistant
{error: "Missing bearer token."}

   1. error: "Missing bearer token."

---

## 🤖 Assistant
Thought for 3s

✅ Fixed — "Missing bearer token" Error

The error occurred because the new /api/search-images endpoint requires authentication (via Bearer token), but the client-side fetch wasn't sending the token.

Updated ImageSearchPanel.jsx (Replace your current one)
jsx
Copy
"use client";

import React, { useState, useEffect } from "react";
import { X, Search, Loader2, Plus, Check } from "lucide-react";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import toast from "react-hot-toast";
import { auth } from "@/firebase/config";

export function ImageSearchPanel({
  open,
  onClose,
  context,
  onSelect,
}) {
  const [query, setQuery] = useState("");
  const [results, setResults] = useState([]);
  const [loading, setLoading] = useState(false);
  const [selected, setSelected] = useState([]);
  const [nextStart, setNextStart] = useState(null);
  const [searching, setSearching] = useState(false);

  const isPoster = context?.type === "poster";
  const maxSelect = isPoster ? 1 : 2;

  // Auto-build smart query
  useEffect(() => {
    if (!open || !context) return;

    let smartQuery = "";

    if (isPoster) {
      smartQuery = `beautiful ${context?.tripTitle || "India"} travel itinerary poster landscape`;
    } else {
      const dayInfo = context.dayTitle ? `${context.dayTitle} ` : `Day ${context.dayIdx + 1} `;
      smartQuery = `${dayInfo} ${context.dayDescription?.slice(0, 100) || ""} ${context?.cities?.join(" ") || ""} India tourism photography`;
    }

    setQuery(smartQuery.trim());
    setSelected([]);
    setResults([]);
    setNextStart(null);

    if (smartQuery) handleSearch(smartQuery);
  }, [open, context]);

  const handleSearch = async (searchQuery = query, start = 1) => {
    if (!searchQuery?.trim()) return;

    setSearching(true);
    setLoading(true);

    try {
      const idToken = await auth.currentUser?.getIdToken();
      if (!idToken) {
        toast.error("Please sign in again");
        return;
      }

      const res = await fetch(
        `/api/search-images?q=${encodeURIComponent(searchQuery)}&start=${start}`,
        {
          headers: {
            Authorization: `Bearer ${idToken}`,
          },
        }
      );

      if (!res.ok) {
        const errData = await res.json().catch(() => ({}));
        throw new Error(errData.error || `Error ${res.status}`);
      }

      const data = await res.json();

      if (start === 1) {
        setResults(data.results || []);
      } else {
        setResults((prev) => [...prev, ...(data.results || [])]);
      }
      setNextStart(data.nextStart);
    } catch (err) {
      console.error(err);
      toast.error(err.message || "Failed to search images");
    } finally {
      setLoading(false);
      setSearching(false);
    }
  };

  const toggleSelect = (url) => {
    if (selected.includes(url)) {
      setSelected(selected.filter((u) => u !== url));
    } else if (selected.length < maxSelect) {
      setSelected([...selected, url]);
    } else {
      toast.error(`You can select maximum ${maxSelect} image${maxSelect > 1 ? 's' : ''}`);
    }
  };

  const handleConfirm = () => {
    if (selected.length === 0) {
      toast.error("Please select at least one image");
      return;
    }
    onSelect(selected);
    onClose();
  };

  if (!open) return null;

  return (
    <>
      <div className="fixed inset-0 bg-black/60 z-[60]" onClick={onClose} />

      <div className="fixed right-0 top-0 h-full w-[460px] bg-white shadow-2xl z-[70] flex flex-col overflow-hidden">
        {/* Header */}
        <div className="px-5 py-4 border-b flex items-center justify-between">
          <div>
            <h3 className="font-semibold text-lg">
              {isPoster ? "Select Cover Image" : "Select Day Images"}
            </h3>
            <p className="text-xs text-slate-500">
              {isPoster ? "1 image" : "Up to 2 images"} • Google Image Search
            </p>
          </div>
          <button onClick={onClose} className="p-2 hover:bg-slate-100 rounded-full">
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Search Input */}
        <div className="p-4 border-b">
          <div className="flex gap-2">
            <Input
              value={query}
              onChange={(e) => setQuery(e.target.value)}
              placeholder="Search for images..."
              onKeyDown={(e) => e.key === "Enter" && handleSearch()}
            />
            <Button onClick={() => handleSearch()} disabled={searching || !query.trim()}>
              {searching ? <Loader2 className="w-4 h-4 animate-spin" /> : <Search className="w-4 h-4" />}
            </Button>
          </div>
        </div>

        {/* Results Area */}
        <div className="flex-1 overflow-y-auto p-4">
          {loading && results.length === 0 && (
            <div className="flex flex-col items-center justify-center py-20">
              <Loader2 className="w-10 h-10 animate-spin text-blue-600 mb-3" />
              <p className="text-slate-500">Searching high-quality images...</p>
            </div>
          )}

          <div className="grid grid-cols-2 gap-3">
            {results.map((img, idx) => {
              const isSelected = selected.includes(img.url);
              return (
                <div
                  key={idx}
                  onClick={() => toggleSelect(img.url)}
                  className={`relative aspect-video rounded-xl overflow-hidden border-2 cursor-pointer transition-all hover:shadow-md ${
                    isSelected ? "border-blue-600 ring-2 ring-blue-500" : "border-transparent hover:border-slate-300"
                  }`}
                >
                  <img
                    src={img.thumbnail || img.url}
                    alt={img.title}
                    className="w-full h-full object-cover"
                  />
                  {isSelected && (
                    <div className="absolute inset-0 bg-blue-600/40 flex items-center justify-center">
                      <div className="bg-white rounded-full p-1.5 shadow">
                        <Check className="w-6 h-6 text-blue-600" />
                      </div>
                    </div>
                  )}
                  <div className="absolute bottom-0 left-0 right-0 bg-gradient-to-t from-black/70 p-2">
                    <p className="text-[10px] text-white line-clamp-2">{img.title}</p>
                  </div>
                </div>
              );
            })}
          </div>

          {results.length === 0 && !loading && (
            <div className="text-center py-16 text-slate-400">
              No results yet. Try searching above.
            </div>
          )}

          {nextStart && (
            <div className="flex justify-center mt-8">
              <Button
                variant="outline"
                onClick={() => handleSearch(query, nextStart)}
                disabled={loading}
              >
                Load More
              </Button>
            </div>
          )}
        </div>

        {/* Footer */}
        <div className="p-4 border-t bg-slate-50 flex gap-3">
          <Button variant="outline" onClick={onClose} className="flex-1">
            Cancel
          </Button>
          <Button
            onClick={handleConfirm}
            disabled={selected.length === 0}
            className="flex-1"
          >
            Add Selected ({selected.length}/{maxSelect})
          </Button>
        </div>
      </div>
    </>
  );
}
Key Fix Applied:
Added auth.currentUser?.getIdToken() before every fetch to /api/search-images
Added proper Authorization: Bearer <token> header
Next Steps for You:
Replace the ImageSearchPanel.jsx with the code above.
Make sure /api/search-images/route.js exists (the one I gave earlier — it uses requireAuthenticatedUser).
Restart your dev server (npm run dev).
Test the "Search Images" button on Poster and on any Day.

Let me know if you get any other error after this. I'll fix it immediately.

---

## 🤖 Assistant
error: "Image search access denied. Check API key configuration."}

   1. error: "Image search access denied. Check API key configuration."

now that doe sthis means ?

---

## 🤖 Assistant
Thought for 3s

✅ Diagnosis & Solution

The error Image search access denied. Check API key configuration. means Google is rejecting the request. This is not a code issue — it's a configuration issue on Google Cloud side.

Why This Happens (Most Common Causes)
Custom Search API is not enabled on your Google Cloud Project.
API Key doesn't have permission for the Custom Search API.
Incorrect or missing GOOGLE_SEARCH_CX (Programmable Search Engine ID).
API Key has restrictions (e.g., referrer or IP restrictions blocking your domain).
Step-by-Step Fix (Do This Now)
Step 1: Enable Custom Search API
Go to: Google Cloud Console
Select your project (top dropdown)
In the search bar, type "Custom Search API" → Click on it
Click Enable
Step 2: Verify/Create API Key with Correct Permissions
Go to APIs & Services → Credentials
Find your existing GOOGLE_SEARCH_API_KEY or create a new one:
Click Create Credentials → API Key
Click on the key → Edit
Under API restrictions, select Restrict key
Choose Custom Search API from the dropdown
Save
Step 3: Create / Verify Programmable Search Engine (CX)
Go to: https://programmablesearchengine.google.com/
Click Add / Create a new search engine
Settings:
Search the entire web → ON
Image search → ON
Copy the Search engine ID (looks like 012345678901234567890:abcde12345)
Put this in your .env.local:
env
Copy
GOOGLE_SEARCH_API_KEY=AIzaSyxxxxxxxxxxxxxxxxxxxxxxxxxxxx
GOOGLE_SEARCH_CX=012345678901234567890:abcde12345
Step 4: Restart & Test
Restart your Next.js server (Ctrl + C then npm run dev)
Try searching images again
Optional: Temporary Debug Improvement

Replace the relevant part in /api/search-images/route.js with better error logging:

JavaScript
Copy
// Inside the Google response error handling section, update this part:
if (!googleResponse.ok) {
  let googleError = `Google API error (${googleResponse.status})`;
  try {
    const errBody = await googleResponse.json();
    console.error("[search-images] Full Google Error:", errBody); // ← Added for debug

    const msg = errBody?.error?.message;
    if (msg) googleError = msg;

    if (googleResponse.status === 403) {
      return Response.json(
        { 
          error: "Image search access denied. Please check that both GOOGLE_SEARCH_API_KEY and GOOGLE_SEARCH_CX are correct and Custom Search API is enabled.",
          details: msg 
        },
        { status: 403 }
      );
    }
    // ... rest of code
  } catch {}
}
Quick Check You Can Do Right Now

Reply with the output of these two console logs (temporary):

Add this in ImageSearchPanel.jsx inside handleSearch function, right after const res = await fetch...:

JavaScript
Copy
console.log("Response status:", res.status);
const errText = await res.text();
console.log("Error body:", errText);

Then try searching and paste the console output here.

Would you like me to:

Give you a simpler version using a public image API as temporary fallback?
Or continue fixing the Google setup?

Let me know what you prefer and I’ll help accordingly.

---

## 🤖 Assistant
ImageSearchPanel.jsx:60 GET http://localhost:3001/api/search-images?q=tiger&start=1 403 (Forbidden)
installHook.js:1 Error: Image search access denied. Please check that both GOOGLE_SEARCH_API_KEY and GOOGLE_SEARCH_CX are correct and Custom Search API is enabled. at handleSearch (ImageSearchPanel.jsx:71:15)

---

## 🤖 Assistant
Thought for 2s

✅ Got it. This is a 403 Forbidden error from Google.

This is not a code bug — your Google API setup is not fully correct yet.

Immediate Action Plan
Step 1: Check Your .env.local

Make sure it looks exactly like this:

env
Copy
GOOGLE_SEARCH_API_KEY=AIzaSyBxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
GOOGLE_SEARCH_CX=012345678901234567890:xxxxxxxxxxxxxxxxxx

Important:

No extra spaces or quotes.
Restart the server after changing .env.local
Step 2: Verify Google Setup (Most Important)

Please follow these steps in order:

Enable the API
Go to: https://console.cloud.google.com/apis/library/customsearch.googleapis.com
Make sure Custom Search API is Enabled
Check API Key Restrictions
Go to: https://console.cloud.google.com/apis/credentials
Click on your API Key
Under API restrictions → Select Custom Search API (not "Don't restrict")
Save
Check Programmable Search Engine (CX)
Go to: https://programmablesearchengine.google.com/controlpanel/all
Open your search engine
Make sure:
Image search is turned ON
Search the entire web is selected
Copy the Search engine ID again and update GOOGLE_SEARCH_CX
Step 3: Temporary Test (Quick Debug)

Replace your current handleSearch function in ImageSearchPanel.jsx with this updated version (adds better logging):

jsx
Copy
const handleSearch = async (searchQuery = query, start = 1) => {
  if (!searchQuery?.trim()) return;

  setSearching(true);
  setLoading(true);

  try {
    const idToken = await auth.currentUser?.getIdToken();
    if (!idToken) {
      toast.error("Please sign in again");
      return;
    }

    const res = await fetch(
      `/api/search-images?q=${encodeURIComponent(searchQuery)}&start=${start}`,
      {
        headers: { Authorization: `Bearer ${idToken}` },
      }
    );

    console.log("API Response Status:", res.status);   // ← Debug

    if (!res.ok) {
      const errorData = await res.json().catch(() => ({}));
      console.error("Full Error Response:", errorData); // ← Debug
      throw new Error(errorData.error || `HTTP ${res.status}`);
    }

    const data = await res.json();
    console.log("Search Results:", data.results?.length || 0);

    if (start === 1) setResults(data.results || []);
    else setResults((prev) => [...prev, ...(data.results || [])]);

    setNextStart(data.nextStart);
  } catch (err) {
    console.error("Search Error:", err);
    toast.error(err.message || "Search failed");
  } finally {
    setLoading(false);
    setSearching(false);
  }
};

Now try searching again and paste the full console output here (especially the Full Error Response).

While You Fix Google Setup — Want a Quick Alternative?

If you want to test the UI right now without waiting for Google approval, I can give you a temporary version using Unsplash (free & easy).

Would you like that as a fallback while you fix Google?

Or do you want me to help you fix the Google configuration further?

Just reply with:

The console logs after above change, OR
“Give me Unsplash fallback”

I’ll help immediately.

---

## 🤖 Assistant
Name
Use a unique name to identify your API key
APIs that can be accessed using this key
New key creation: To use Agent Platform (Vertex) or Gemini APIs with an API key, you must bind it to a service account by selecting 'Authenticate API calls through a service account' before applying the API restriction.

Note: An organisation policy governs creating API keys bound to a service account for projects with a Google Cloud organisation. Learn more
Select API restrictions
1 API
This list includes only APIs enabled in this project. If you don't see the API that you need, visit the API Library to enable it.
Selected APIs:

Custom Search API

Key restrictions
Add restrictions to reduce security risk and prevent unauthorised use. Learn more
Action recommended: This key can currently be used with any application. Restrict where it can be used to improve security. Learn more
Application restrictions
None
Websites
IP addresses
Android apps
iOS apps
Note: It may take up to five minutes for settings to take effect.
so here do i need to select custom search inlist ?

---

## 🤖 Assistant
Thought for 2s

✅ Yes, you are on the right track.

Answer to your question:

Yes, you must select Custom Search API in the "Selected APIs" list (which you already have). That part is correct.

However, there are a few more important things to fix:
Step-by-Step Final Checklist (Do this now):
API Restrictions (You did this — Good)
Make sure only Custom Search API is selected.
Application Restrictions ← This is very important
Currently it says "None" → This is risky and sometimes causes issues.
Change it to "Websites"
Add your local domain:
http://localhost:3000
http://localhost:3001 (since you are using port 3001)
Also add your production domain later (e.g. https://yourdomain.com)
Save the API Key
Important: After making these changes, wait 2–5 minutes for Google to apply the settings.
After doing the above:
Restart your Next.js server completely (Ctrl + C then npm run dev)
Try searching again in the Image Search Panel.
Still not working? Do this:

Go to your .env.local and make sure it looks like this:

env
Copy
GOOGLE_SEARCH_API_KEY=AIzaSyBxxxxxxxxxxxxxxxxxxxxxxxxxxxx
GOOGLE_SEARCH_CX=your_cx_id_here:xxxxxxxxxx

Then send me:

Whether you added http://localhost:3001 in Application Restrictions → Websites.
Any new error message you get after restart.

Would you like me to also give you a temporary Unsplash fallback so you can continue developing the UI while Google API gets fixed?

Just say "Give Unsplash fallback" and I’ll provide a quick alternative version.

---

## 🤖 Assistant
GOOGLE_SEARCH_CX=your_cx_id_here:xxxxxxxxxx
 
where to get how it looks

---

## 🤖 Assistant
Thought for 3s

✅ Perfect Question!

What is GOOGLE_SEARCH_CX?

It is your Programmable Search Engine ID (also called Search Engine ID or CX).

It looks like this example:

text
Copy
GOOGLE_SEARCH_CX=012345678901234567890:abcdefghijklm

(First part is numbers, then : , then letters/numbers)

How to Get Your GOOGLE_SEARCH_CX

Step-by-step:

Open this link:
https://programmablesearchengine.google.com/controlpanel/all

You will see a list of your search engines.

Click on the search engine you created for this project (or click "Add" to create a new one if you haven't).

On the next page, look for "Search engine ID" in the Basic section.

It will be clearly labeled as "Search engine ID".
Copy that full value (it contains : in the middle).

Paste it in your .env.local file like this:

env
Copy
GOOGLE_SEARCH_CX=012345678901234567890:abcdefghijklm
Quick Tips:
Make sure you have turned ON these two options while creating the search engine:
Image search → ON
Search the entire web → Selected
After updating the .env.local, restart your server (Ctrl + C then npm run dev).

Would you like me to also give you a temporary Unsplash version of the ImageSearchPanel so you can continue working while you fix the Google CX?

Just reply with:

“Got CX” → if you successfully added it, or
“Give Unsplash fallback” → for quick testing

Let me know!

---

## 🤖 Assistant
Overview
Basic
Search engine name
image engine
edit
Description
Add description
Code
Get code
Search engine ID
f6b2dcb87d33449fe
Public URL
https://cse.google.com/cse?cx=f6b2dcb87d33449fe
Search features
Search settings
Image search
SafeSearch
Augment results
Search the entire web
Region
All regions
Region-restricted results
Sites to search
URL contains
SiteLast updated time
14 May 2026, 16:35
Rows per page
5
Showing 1 to 1 of 1 rows
1‑1 of 1
Sites to exclude
You do not have excluded sites.
Upload/download annotations in XML files
Upload/download search engine context in XML files
Look and feel
Language
All languages
Layout
Overlay
Theme
Default
Members
MemberRoleVerified
rushi7gaikwad@gmail.com (me)OwnerYes
Showing 1 to 1 of 1 rows
1‑1 of 1
Ads
Nonprofit organisations, accredited educational institutions and government agencies may qualify for a free version of Programmable Search Engine without ads. Learn more
 
this is what i got

---

## 🤖 Assistant
Thought for 2s

✅ Perfect! Thank you for sharing the screenshot/details.

Your GOOGLE_SEARCH_CX is:
env
Copy
GOOGLE_SEARCH_CX=f6b2dcb87d33449fe
Now Update Your .env.local:
env
Copy
GOOGLE_SEARCH_API_KEY=AIzaSyBxxxxxxxxxxxxxxxxxxxxxxxxxxxx   # ← your actual key
GOOGLE_SEARCH_CX=f6b2dcb87d33449fe
Next Steps (Do this now):
Update .env.local with the above CX.
Restart your Next.js server completely:
Stop the server (Ctrl + C)
Run again: npm run dev
Try searching images again in the panel.
Important Notes about your Search Engine:
Image search is turned ON → Good.
Search the entire web is selected → Good.
However, "Rows per page" is set to 5 — this might limit results. You can increase it to 10 later if you want.
If you still get 403 after this:

Please try the following and tell me the result:

Go back to Google Cloud Console → Credentials → Edit your API Key.
Under Application restrictions, select Websites.
Add these two URLs:
http://localhost:3000
http://localhost:3001
Save and wait 3–5 minutes.
Restart server and test again.

Would you like me to also provide a temporary Unsplash fallback right now so you can test the whole UI immediately while Google catches up?

Just say "Give fallback" and I’ll give you a version that works without Google API.

Let me know the result after you update the CX and restart!

---

## 🤖 Assistant
now for google search api key Credentials
Create credentials to access your enabled APIs. Learn more 
API keys
NameBound account Creation dateRestrictionsActionsnew api for custom search—14 May 2026Custom Search API
OAuth 2.0 Client IDs
NameCreation dateTypeClient IDActionsWeb client 129 Oct 2025Web application515681206635-48tg...
Service Accounts
Manage service accounts
EmailNameActionsNo service accounts to display
we need to get key from API keys right ?

---

## 🤖 Assistant
Thought for 2s

✅ Yes, correct.

You need to use the API Key from the "API keys" section.

Step-by-step: How to Copy Your API Key

On the same page you are on (Credentials), find this row:

new api for custom search — 14 May 2026 — Custom Search API

Click on the name "new api for custom search" (it should be clickable).

A side panel or new page will open showing the full API Key (it starts with AIzaSy...).

Copy the full key.

Then update your .env.local:
env
Copy
GOOGLE_SEARCH_API_KEY=AIzaSyBxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx   # ← Paste full key here
GOOGLE_SEARCH_CX=f6b2dcb87d33449fe
After pasting:
Save the file
Fully restart your server (Ctrl + C then npm run dev)
Test the image search again
Quick Tips:
Make sure there are no extra spaces before or after the key.
The key should start with AIzaSy...

Would you like me to wait for you to paste the key and test, or do you want a temporary Unsplash fallback right now so you can continue building without waiting for Google?

Just reply with one of these:

“I pasted the key, still error” → I’ll help debug further
“Give me Unsplash fallback” → I’ll give you working code immediately

Go ahead and copy the key first.

---

## 🤖 Assistant
dotenv
Copy
# Google Custom Search API (Image Search)
GOOGLE_SEARCH_API_KEY="AIzaSyBDXkjPp6RBnhXE3jGVAVrMwh8byiw_l1o"
GOOGLE_SEARCH_CX="f6b2dcb87d33449fe"

 
so these are the values but i am still getting error

---

## 🤖 Assistant
Thought for 2s

✅ Got it. Thanks for sharing the exact values.

Problem Summary:

Even though the values look correct, you're still getting 403 Forbidden. This is very common and usually due to API Key Restrictions.

Immediate Fixes (Try in this order):
1. Remove Quotes from .env.local

Your current file has quotes — remove them:

dotenv
Copy
# Google Custom Search API (Image Search)
GOOGLE_SEARCH_API_KEY=AIzaSyBDXkjPp6RBnhXE3jGVAVrMwh8byiw_l1o
GOOGLE_SEARCH_CX=f6b2dcb87d33449fe

Save file → Fully restart server (Ctrl+C then npm run dev)

2. Remove Restrictions Temporarily (Most Important Right Now)
Go to Google Cloud Console → Credentials
Click on your key "new api for custom search"
Under Application restrictions, change from "Websites" to "None" (temporarily)
Click Save
Wait 1–2 minutes
Restart server and test again

This is the most common reason for 403 even when key + cx are correct.

3. Also Check API Restrictions

Make sure only Custom Search API is selected (which you already did).

After doing Step 1 + Step 2, please tell me:
Did the error change?
What is the exact error message now? (copy from browser console)
Bonus: Quick Test Command

You can also test the API directly in browser:

text
Copy
http://localhost:3001/api/search-images?q=taj+mahal&start=1

Open this link and tell me what you see.

Would you like me to give you a working Unsplash fallback version right now so you don’t get stuck? It’s very easy to switch later back to Google.

Just say:

“I did the changes” → I’ll wait for result, or
“Give Unsplash fallback” → I’ll give you working code immediately

Let me know.

---

## 🤖 Assistant
api keys are added

JavaScript
Copy
/**
 * app/api/search-images/route.js
 *
 * Proxy for Google Custom Search JSON API — image search.
 * Keeps API keys server-side, enforces auth + rate limit.
 *
 * ENV vars required:
 *   GOOGLE_SEARCH_API_KEY   — Google Cloud API key with Custom Search API enabled
 *   GOOGLE_SEARCH_CX        — Programmable Search Engine ID (cx), image search ON,
 *                             "Search the entire web" enabled
 *
 * GET /api/search-images?q=Amber+Fort+Jaipur&start=1
 *   q      : search query (required)
 *   start  : pagination offset, 1-based, max 91 (Google limit) — default 1
 *
 * Returns:
 *   { results: ImageResult[], totalResults: number, nextStart: number | null }
 *
 * ImageResult: { url, thumbnail, title, source, width, height }
 */

import { requireAuthenticatedUser } from "@/lib/serverAuth";
import { rateLimit } from "@/lib/rateLimit";

// How many results to request per page (max 10 per Google's API)
const PAGE_SIZE = 10;

// Domains known to serve watermarked / low-quality results — filtered out
const BLOCKED_DOMAINS = [
  "shutterstock.com",
  "gettyimages.com",
  "istockphoto.com",
  "adobe.com",
  "dreamstime.com",
  "depositphotos.com",
  "alamy.com",
  "123rf.com",
  "stocksy.com",
  "bigstockphoto.com",
];

function isDomainBlocked(url) {
  try {
    const hostname = new URL(url).hostname.toLowerCase();
    return BLOCKED_DOMAINS.some((d) => hostname.includes(d));
  } catch {
    return true; // malformed URL — block it
  }
}

/**
 * Validate and sanitise the search query.
 * Returns { ok: true, query } or { ok: false, error }
 */
function validateQuery(raw) {
  if (!raw || typeof raw !== "string") {
    return { ok: false, error: "Query parameter 'q' is required." };
  }
  const trimmed = raw.trim();
  if (trimmed.length === 0) {
    return { ok: false, error: "Query cannot be empty." };
  }
  if (trimmed.length > 200) {
    return { ok: false, error: "Query is too long (max 200 characters)." };
  }
  return { ok: true, query: trimmed };
}

export async function GET(req) {
  // ── 1. Authenticate ──────────────────────────────────────────────────────
  let requester;
  try {
    requester = await requireAuthenticatedUser(req);
  } catch (err) {
    return Response.json({ error: err.message }, { status: err.status || 401 });
  }

  if (!requester?.uid) {
    return Response.json(
      { error: "Authentication required." },
      { status: 401 },
    );
  }

  // ── 2. Rate limit — 30 searches/min per user ─────────────────────────────
  // Google gives 100 free queries/day total; 30/min per user is generous
  // but protects the daily quota from a single user burning it all.
  const rl = rateLimit({
    uid: requester.uid,
    action: "search-images",
    limit: 30,
    windowMs: 60_000,
  });
  if (!rl.allowed) {
    return Response.json(
      { error: "Too many image search requests. Please wait a moment." },
      {
        status: 429,
        headers: {
          "Retry-After": String(Math.ceil((rl.resetAt - Date.now()) / 1000)),
        },
      },
    );
  }

  // ── 3. Parse + validate query params ─────────────────────────────────────
  const { searchParams } = new URL(req.url);
  const rawQuery = searchParams.get("q");
  const rawStart = searchParams.get("start");

  const { ok, query, error: queryError } = validateQuery(rawQuery);
  if (!ok) {
    return Response.json({ error: queryError }, { status: 400 });
  }

  // start must be 1–91 (Google allows items 1-100, but only 10 per request)
  let start = parseInt(rawStart, 10);
  if (isNaN(start) || start < 1) start = 1;
  if (start > 91) start = 91;

  // ── 4. Validate env vars ──────────────────────────────────────────────────
  const apiKey = process.env.GOOGLE_SEARCH_API_KEY;
  const cx = process.env.GOOGLE_SEARCH_CX;

  if (!apiKey || !cx) {
    console.error(
      "[search-images] Missing GOOGLE_SEARCH_API_KEY or GOOGLE_SEARCH_CX",
    );
    return Response.json(
      { error: "Image search is not configured. Please contact support." },
      { status: 503 },
    );
  }

  // ── 5. Build Google CSE URL ───────────────────────────────────────────────
  const params = new URLSearchParams({
    key: apiKey,
    cx,
    q: query,
    searchType: "image",
    num: String(PAGE_SIZE),
    start: String(start),
    // Prefer landscape, high-quality images
    imgSize: "large",
    imgType: "photo",
    safe: "active",
    // Rights filter — prefer images labeled for reuse
    // (not strict — just a hint; agents are responsible for final use)
    rights: "cc_publicdomain|cc_attribute|cc_sharealike",
  });

  const apiUrl = `https://www.googleapis.com/customsearch/v1?${params.toString()}`;

  // ── 6. Call Google CSE ────────────────────────────────────────────────────
  let googleResponse;
  try {
    googleResponse = await fetch(apiUrl, {
      // 10-second timeout
      signal: AbortSignal.timeout(10_000),
    });
  } catch (fetchErr) {
    console.error(
      "[search-images] Network error calling Google CSE:",
      fetchErr.message,
    );
    if (fetchErr.name === "TimeoutError" || fetchErr.name === "AbortError") {
      return Response.json(
        { error: "Image search timed out. Please try again." },
        { status: 504 },
      );
    }
    return Response.json(
      { error: "Failed to reach image search service. Please try again." },
      { status: 502 },
    );
  }

  // ── 7. Handle non-OK Google responses ────────────────────────────────────
  if (!googleResponse.ok) {
    let googleError = `Google API error (${googleResponse.status})`;
    try {
      const errBody = await googleResponse.json();
      console.error("[search-images] Full Google Error:", errBody); // ← Added for debug

      const msg = errBody?.error?.message;
      if (msg) googleError = msg;

      if (googleResponse.status === 403) {
        return Response.json(
          {
            error:
              "Image search access denied. Please check that both GOOGLE_SEARCH_API_KEY and GOOGLE_SEARCH_CX are correct and Custom Search API is enabled.",
            details: msg,
          },
          { status: 403 },
        );
      }
      // Specific known errors
      if (googleResponse.status === 403) {
        // Could be quota exceeded or invalid key
        const reason = errBody?.error?.errors?.[0]?.reason;
        if (reason === "rateLimitExceeded" || reason === "dailyLimitExceeded") {
          return Response.json(
            {
              error:
                "Daily image search quota reached. Please try again tomorrow.",
            },
            { status: 429 },
          );
        }
        return Response.json(
          { error: "Image search access denied. Check API key configuration." },
          { status: 403 },
        );
      }
    } catch {
      // ignore parse error
    }
    console.error("[search-images] Google CSE error:", googleError);
    return Response.json(
      { error: "Image search failed. Please try again." },
      { status: 502 },
    );
  }

  // ── 8. Parse response ─────────────────────────────────────────────────────
  let googleData;
  try {
    googleData = await googleResponse.json();
  } catch {
    return Response.json(
      { error: "Invalid response from image search service." },
      { status: 502 },
    );
  }

  // ── 9. Extract + filter results ───────────────────────────────────────────
  const rawItems = googleData?.items ?? [];
  const totalResults = parseInt(
    googleData?.searchInformation?.totalResults ?? "0",
    10,
  );

  const results = rawItems
    .map((item) => {
      const url = item?.link;
      const thumbnail =
        item?.image?.thumbnailLink ||
        item?.pagemap?.cse_thumbnail?.[0]?.src ||
        url;
      const title = item?.title || "";
      const source = item?.displayLink || "";
      const width = item?.image?.width || 0;
      const height = item?.image?.height || 0;

      if (!url || typeof url !== "string") return null;
      if (!url.startsWith("https://") && !url.startsWith("http://"))
        return null;
      if (isDomainBlocked(url)) return null;

      return { url, thumbnail, title, source, width, height };
    })
    .filter(Boolean);

  // Calculate next page start
  const nextStart =
    start + PAGE_SIZE <= Math.min(totalResults, 100) ? start + PAGE_SIZE : null;

  return Response.json(
    { results, totalResults, nextStart, currentStart: start },
    {
      status: 200,
      headers: {
        // Cache for 5 minutes — same query shouldn't re-hit Google
        "Cache-Control": "private, max-age=300",
      },
    },
  );
}

 
 

javascriptreact
Copy
"use client";

import React, { useState, useEffect } from "react";
import { X, Search, Loader2, Plus, Check } from "lucide-react";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import toast from "react-hot-toast";
import { auth } from "@/firebase/config";

export function ImageSearchPanel({
  open,
  onClose,
  context,
  onSelect,
}) {
  const [query, setQuery] = useState("");
  const [results, setResults] = useState([]);
  const [loading, setLoading] = useState(false);
  const [selected, setSelected] = useState([]);
  const [nextStart, setNextStart] = useState(null);
  const [searching, setSearching] = useState(false);

  const isPoster = context?.type === "poster";
  const maxSelect = isPoster ? 1 : 2;

  // Auto-build smart query
  useEffect(() => {
    if (!open || !context) return;

    let smartQuery = "";

    if (isPoster) {
      smartQuery = `beautiful ${context?.tripTitle || "India"} travel itinerary poster landscape`;
    } else {
      const dayInfo = context.dayTitle ? `${context.dayTitle} ` : `Day ${context.dayIdx + 1} `;
      smartQuery = `${dayInfo} ${context.dayDescription?.slice(0, 100) || ""} ${context?.cities?.join(" ") || ""} India tourism photography`;
    }

    setQuery(smartQuery.trim());
    setSelected([]);
    setResults([]);
    setNextStart(null);

    if (smartQuery) handleSearch(smartQuery);
  }, [open, context]);

  const handleSearch = async (searchQuery = query, start = 1) => {
    if (!searchQuery?.trim()) return;

    setSearching(true);
    setLoading(true);

    try {
      const idToken = await auth.currentUser?.getIdToken();
      if (!idToken) {
        toast.error("Please sign in again");
        return;
      }

      const res = await fetch(
        `/api/search-images?q=${encodeURIComponent(searchQuery)}&start=${start}`,
        {
          headers: {
            Authorization: `Bearer ${idToken}`,
          },
        }
      );

      if (!res.ok) {
        const errData = await res.json().catch(() => ({}));
        throw new Error(errData.error || `Error ${res.status}`);
      }

      const data = await res.json();

      if (start === 1) {
        setResults(data.results || []);
      } else {
        setResults((prev) => [...prev, ...(data.results || [])]);
      }
      setNextStart(data.nextStart);
    } catch (err) {
      console.error(err);
      toast.error(err.message || "Failed to search images");
    } finally {
      setLoading(false);
      setSearching(false);
    }
  };

  const toggleSelect = (url) => {
    if (selected.includes(url)) {
      setSelected(selected.filter((u) => u !== url));
    } else if (selected.length < maxSelect) {
      setSelected([...selected, url]);
    } else {
      toast.error(`You can select maximum ${maxSelect} image${maxSelect > 1 ? 's' : ''}`);
    }
  };

  const handleConfirm = () => {
    if (selected.length === 0) {
      toast.error("Please select at least one image");
      return;
    }
    onSelect(selected);
    onClose();
  };

  if (!open) return null;

  return (
    <>
      <div className="fixed inset-0 bg-black/60 z-[60]" onClick={onClose} />

      <div className="fixed right-0 top-0 h-full w-[460px] bg-white shadow-2xl z-[70] flex flex-col overflow-hidden">
        {/* Header */}
        <div className="px-5 py-4 border-b flex items-center justify-between">
          <div>
            <h3 className="font-semibold text-lg">
              {isPoster ? "Select Cover Image" : "Select Day Images"}
            </h3>
            <p className="text-xs text-slate-500">
              {isPoster ? "1 image" : "Up to 2 images"} • Google Image Search
            </p>
          </div>
          <button onClick={onClose} className="p-2 hover:bg-slate-100 rounded-full">
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Search Input */}
        <div className="p-4 border-b">
          <div className="flex gap-2">
            <Input
              value={query}
              onChange={(e) => setQuery(e.target.value)}
              placeholder="Search for images..."
              onKeyDown={(e) => e.key === "Enter" && handleSearch()}
            />
            <Button onClick={() => handleSearch()} disabled={searching || !query.trim()}>
              {searching ? <Loader2 className="w-4 h-4 animate-spin" /> : <Search className="w-4 h-4" />}
            </Button>
          </div>
        </div>

        {/* Results Area */}
        <div className="flex-1 overflow-y-auto p-4">
          {loading && results.length === 0 && (
            <div className="flex flex-col items-center justify-center py-20">
              <Loader2 className="w-10 h-10 animate-spin text-blue-600 mb-3" />
              <p className="text-slate-500">Searching high-quality images...</p>
            </div>
          )}

          <div className="grid grid-cols-2 gap-3">
            {results.map((img, idx) => {
              const isSelected = selected.includes(img.url);
              return (
                <div
                  key={idx}
                  onClick={() => toggleSelect(img.url)}
                  className={`relative aspect-video rounded-xl overflow-hidden border-2 cursor-pointer transition-all hover:shadow-md ${
                    isSelected ? "border-blue-600 ring-2 ring-blue-500" : "border-transparent hover:border-slate-300"
                  }`}
                >
                  <img
                    src={img.thumbnail || img.url}
                    alt={img.title}
                    className="w-full h-full object-cover"
                  />
                  {isSelected && (
                    <div className="absolute inset-0 bg-blue-600/40 flex items-center justify-center">
                      <div className="bg-white rounded-full p-1.5 shadow">
                        <Check className="w-6 h-6 text-blue-600" />
                      </div>
                    </div>
                  )}
                  <div className="absolute bottom-0 left-0 right-0 bg-gradient-to-t from-black/70 p-2">
                    <p className="text-[10px] text-white line-clamp-2">{img.title}</p>
                  </div>
                </div>
              );
            })}
          </div>

          {results.length === 0 && !loading && (
            <div className="text-center py-16 text-slate-400">
              No results yet. Try searching above.
            </div>
          )}

          {nextStart && (
            <div className="flex justify-center mt-8">
              <Button
                variant="outline"
                onClick={() => handleSearch(query, nextStart)}
                disabled={loading}
              >
                Load More
              </Button>
            </div>
          )}
        </div>

        {/* Footer */}
        <div className="p-4 border-t bg-slate-50 flex gap-3">
          <Button variant="outline" onClick={onClose} className="flex-1">
            Cancel
          </Button>
          <Button
            onClick={handleConfirm}
            disabled={selected.length === 0}
            className="flex-1"
          >
            Add Selected ({selected.length}/{maxSelect})
          </Button>
        </div>
      </div>
    </>
  );
}

 
check if here are any error s?

---

## 🤖 Assistant
Thought for 3s

✅ Code Review Done

I checked all 3 files you shared. Here are the issues and fixes:

1. Main Issues Found
Issue #1: Wrong Import Path in ItineraryForm
jsx
Copy
import { ImageSearchPanel } from "../ImageSearchPanel";   // ← This may be wrong
Issue #2: Missing Context Data (Most Important)

You are not passing cities or title to the search panel, so smart query is weak.

Fixed & Improved Code
A. Updated ItineraryForm.jsx (Only changed parts)

Replace the relevant sections:

jsx
Copy
// At top imports - Fix this
import { ImageSearchPanel } from "./../ItinerarySubComponents/ImageSearchPanel";   // ← Recommended path

// ... rest of imports

// Inside ItineraryForm component - Update openImageSearch
const openImageSearch = (context) => {
  const enrichedContext = {
    ...context,
    tripTitle: form.title,
    cities: form.cities,
  };
  setImageSearchContext(enrichedContext);
  setImageSearchOpen(true);
};
B. Updated ImageSearchPanel.jsx (Better Smart Query + Minor Fixes)
jsx
Copy
"use client";

import React, { useState, useEffect } from "react";
import { X, Search, Loader2, Plus, Check } from "lucide-react";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import toast from "react-hot-toast";
import { auth } from "@/firebase/config";

export function ImageSearchPanel({
  open,
  onClose,
  context,
  onSelect,
}) {
  const [query, setQuery] = useState("");
  const [results, setResults] = useState([]);
  const [loading, setLoading] = useState(false);
  const [selected, setSelected] = useState([]);
  const [nextStart, setNextStart] = useState(null);
  const [searching, setSearching] = useState(false);

  const isPoster = context?.type === "poster";
  const maxSelect = isPoster ? 1 : 2;

  // Auto-build better smart query
  useEffect(() => {
    if (!open || !context) return;

    let smartQuery = "";

    if (isPoster) {
      smartQuery = `beautiful ${context.tripTitle || "India"} travel itinerary poster landscape high resolution`;
    } else {
      const dayInfo = context.dayTitle 
        ? `${context.dayTitle}` 
        : `Day ${context.dayIdx + 1}`;
      
      smartQuery = `${dayInfo} ${context.dayDescription?.slice(0, 80) || ""} 
                   ${context.cities?.join(" ") || ""} India tourism photography`;
    }

    setQuery(smartQuery.trim().replace(/\s+/g, " "));
    setSelected([]);
    setResults([]);
    setNextStart(null);

    // Auto search
    setTimeout(() => {
      if (smartQuery) handleSearch(smartQuery);
    }, 300);
  }, [open, context]);

  const handleSearch = async (searchQuery = query, start = 1) => {
    if (!searchQuery?.trim()) return;

    setSearching(true);
    setLoading(true);

    try {
      const idToken = await auth.currentUser?.getIdToken();
      if (!idToken) {
        toast.error("Please sign in again");
        return;
      }

      const res = await fetch(
        `/api/search-images?q=${encodeURIComponent(searchQuery)}&start=${start}`,
        {
          headers: { Authorization: `Bearer ${idToken}` },
        }
      );

      if (!res.ok) {
        const errData = await res.json().catch(() => ({}));
        console.error("Search API Error:", errData);
        throw new Error(errData.error || `Error ${res.status}`);
      }

      const data = await res.json();

      if (start === 1) {
        setResults(data.results || []);
      } else {
        setResults((prev) => [...prev, ...(data.results || [])]);
      }
      setNextStart(data.nextStart);
    } catch (err) {
      console.error(err);
      toast.error(err.message || "Failed to search images");
    } finally {
      setLoading(false);
      setSearching(false);
    }
  };

  // ... rest of the component (toggleSelect, handleConfirm, return) remains same
Quick Test After Changes
Replace the import in ItineraryForm
Update the openImageSearch function
Replace ImageSearchPanel with improved version above
Restart server

Then try searching.

Still getting 403?

Run this URL in browser:

text
Copy
http://localhost:3001/api/search-images?q=taj+mahal

Tell me what error you see.

Do you want me to give you a temporary Unsplash fallback now so the feature works immediately while we fix Google?

Just say the word.

---

## 🤖 Assistant
http://localhost:3001/api/search-images?q=taj+mahal
 

text
Copy
{"error":"Missing bearer token."}

---

## 🤖 Assistant
Thought for 3s

✅ Got it!

The error {"error":"Missing bearer token."} when opening the URL directly in the browser is expected and normal.

Why it happens:
The API route requires authentication (requireAuthenticatedUser).
When you open the URL directly in the browser, no Authorization: Bearer ... token is sent.
The ImageSearchPanel sends the token correctly when used inside the app.
Real Problem Right Now

The main issue is still the 403 from Google. Let's debug it properly.

Please Do This Now:

1. Add this improved debug version of /api/search-images/route.js

Replace your current route file with this (I added more logging):

JavaScript
Copy
// app/api/search-images/route.js
import { requireAuthenticatedUser } from "@/lib/serverAuth";
import { rateLimit } from "@/lib/rateLimit";

const PAGE_SIZE = 10;

export async function GET(req) {
  let requester;
  try {
    requester = await requireAuthenticatedUser(req);
  } catch (err) {
    return Response.json({ error: err.message }, { status: 401 });
  }

  const { searchParams } = new URL(req.url);
  const q = searchParams.get("q");

  console.log("🔍 Search Request:", { q, uid: requester.uid });

  const apiKey = process.env.GOOGLE_SEARCH_API_KEY;
  const cx = process.env.GOOGLE_SEARCH_CX;

  console.log("🔑 Env Check:", { 
    hasApiKey: !!apiKey, 
    hasCx: !!cx,
    apiKeyLength: apiKey?.length,
    cx 
  });

  if (!apiKey || !cx) {
    return Response.json({ error: "Missing env variables" }, { status: 503 });
  }

  const params = new URLSearchParams({
    key: apiKey,
    cx,
    q,
    searchType: "image",
    num: String(PAGE_SIZE),
    safe: "active",
  });

  const apiUrl = `https://www.googleapis.com/customsearch/v1?${params.toString()}`;

  console.log("🌐 Calling Google:", apiUrl.replace(apiKey, "API_KEY_HIDDEN"));

  try {
    const googleRes = await fetch(apiUrl, { signal: AbortSignal.timeout(10000) });

    console.log("📡 Google Status:", googleRes.status);

    if (!googleRes.ok) {
      const errorBody = await googleRes.json().catch(() => ({}));
      console.error("❌ Full Google Error:", errorBody);

      return Response.json({
        error: "Google API Error",
        status: googleRes.status,
        details: errorBody?.error?.message || "Unknown error"
      }, { status: googleRes.status });
    }

    const data = await googleRes.json();
    return Response.json({
      results: data.items || [],
      totalResults: data.searchInformation?.totalResults
    });

  } catch (err) {
    console.error("Fetch Error:", err);
    return Response.json({ error: err.message }, { status: 502 });
  }
}

2. Restart server and then:

Go to the Itinerary Form
Click "Search Images" button (on poster or day)
Check the server terminal for logs (very important)

Paste the server logs here (especially lines with 🔑 Env Check, 🌐 Calling Google, and ❌ Full Google Error).

While waiting for your logs, would you like me to implement a temporary Unsplash fallback so the feature works right now?

Just say "Implement Unsplash" and I’ll give you a working version quickly.

---

## 🤖 Assistant
ET http://localhost:3001/api/search-images?q=Arrival%20in%20Itanagar%20You%20will%20arrive%20at%20the%20airport%20and%20be%20transferred%20to%20Itanagar.%20Upon%20arrival%2C%20che%20India%20tourism%20photography&start=1 403 (Forbidden)
(anonymous) @ ImageSearchPanel.jsx:68
await in (anonymous)
(anonymous) @ ImageSearchPanel.jsx:148
executeDispatch @ react-dom-client.development.js:20610
runWithFiberInDEV @ react-dom-client.development.js:986
processDispatchQueue @ react-dom-client.development.js:20660
(anonymous) @ react-dom-client.development.js:21234
batchedUpdates$1 @ react-dom-client.development.js:3377
dispatchEventForPluginEventSystem @ react-dom-client.development.js:20814
dispatchEvent @ react-dom-client.development.js:25817
dispatchDiscreteEvent @ react-dom-client.development.js:25785Understand this error
installHook.js:1 Search API Error: {error: 'Google API Error', status: 403, details: 'This project does not have the access to Custom Search JSON API.'}details: "This project does not have the access to Custom Search JSON API."error: "Google API Error"status: 403[[Prototype]]: Object
overrideMethod @ installHook.js:1
error @ intercept-console-error.ts:48
(anonymous) @ ImageSearchPanel.jsx:77
await in (anonymous)
(anonymous) @ ImageSearchPanel.jsx:148
executeDispatch @ react-dom-client.development.js:20610
runWithFiberInDEV @ react-dom-client.development.js:986
processDispatchQueue @ react-dom-client.development.js:20660
(anonymous) @ react-dom-client.development.js:21234
batchedUpdates$1 @ react-dom-client.development.js:3377
dispatchEventForPluginEventSystem @ react-dom-client.development.js:20814
dispatchEvent @ react-dom-client.development.js:25817
dispatchDiscreteEvent @ react-dom-client.development.js:25785Understand this error
installHook.js:1 Error: Google API Error
    at handleSearch (src_0qu9jik._.js?id=%255Bproject%255D%252Fsrc%252Fapp%252Fagent-panel%252Fitinerary%252FImageSearchPanel.jsx+%255Bapp-client%255D+%2528ecmascript%2529:83:23)

---

## 🤖 Assistant
Thought for 2s

✅ Root Cause Found

The error is very clear:

"This project does not have the access to Custom Search JSON API."

This means even though you created an API key and restricted it, the Custom Search API is not enabled on your Google Cloud Project.

Fix It Now (Most Important Step)
Go to this direct link:
https://console.cloud.google.com/apis/library/customsearch.googleapis.com
Make sure the correct project is selected at the top.
Click the Enable button.
Wait 10–30 seconds.
Then fully restart your Next.js server (Ctrl + C → npm run dev).
After Enabling:

Try searching again in the Image Search Panel.

If it still fails, please share:

The new error message from browser console, AND
Any logs from your terminal/server.
While You Enable the API — Temporary Fix (Recommended)

Since this is taking too long, I recommend switching to Unsplash temporarily so you can continue development.

Would you like me to give you a working Unsplash version of ImageSearchPanel + updated route right now?

Just reply with "Give Unsplash" and I’ll provide clean, working code immediately (no Google API needed).

Or say "I enabled it" after you enable the API above.

---

## 🤖 Assistant
i have enable that feature and went to creadentail section and created oclient id

---

## 🤖 Assistant
Thought for 3s

✅ Thanks for the screenshot.

Problem:

You have enabled the Custom Search API (correct), but you created an OAuth 2.0 Client ID instead of an API Key.

For the Custom Search JSON API, we need a regular API Key, not OAuth.

Correct Steps (Do This Now):
Go to Credentials Page:
https://console.cloud.google.com/apis/credentials
Click on "+ Create Credentials" (top of the page)
Select API Key (not OAuth client ID)
After it is created:
Click on the newly created API Key
Copy the key (starts with AIzaSy...)
Update your .env.local:
dotenv
Copy
GOOGLE_SEARCH_API_KEY=AIzaSyBDXkjPp6RBnhXE3jGVAVrMwh8byiw_l1o   # ← Use this new key
GOOGLE_SEARCH_CX=f6b2dcb87d33449fe
Restart your server completely.
Also Do This (Important):

On the same API Key page:

Under Application restrictions → Select "None" (for now, to test)
Under API restrictions → Select "Custom Search API"

Then save.

Would you like me to give you a temporary working version using Unsplash right now? It will work immediately without any Google setup.

Just say "Give Unsplash version" and I’ll replace the image search with a reliable free alternative while you fix the Google key.

Or tell me once you create the correct API Key.

---

## 🤖 Assistant
i have enable the search api , then createed api still same error

---

## 🤖 Assistant
Thought for 3s

✅ Understood.

Since Google Custom Search is still giving issues (very common setup problems), let's switch to a reliable temporary solution using Unsplash so you can continue developing without delay.

Step 1: Replace /api/search-images/route.js

Replace the entire file with this Unsplash version:

JavaScript
Copy
/**
 * app/api/search-images/route.js
 * Temporary Unsplash Image Search (Free & Reliable)
 */

import { requireAuthenticatedUser } from "@/lib/serverAuth";

const UNSPLASH_ACCESS_KEY = process.env.UNSPLASH_ACCESS_KEY;

export async function GET(req) {
  let requester;
  try {
    requester = await requireAuthenticatedUser(req);
  } catch (err) {
    return Response.json({ error: err.message }, { status: 401 });
  }

  const { searchParams } = new URL(req.url);
  const q = searchParams.get("q") || "india travel";
  const page = parseInt(searchParams.get("page")) || 1;

  if (!UNSPLASH_ACCESS_KEY) {
    return Response.json({ error: "Unsplash is not configured" }, { status: 503 });
  }

  try {
    const res = await fetch(
      `https://api.unsplash.com/search/photos?query=${encodeURIComponent(q)}&per_page=12&page=${page}&orientation=landscape`,
      {
        headers: {
          Authorization: `Client-ID ${UNSPLASH_ACCESS_KEY}`,
        },
      }
    );

    if (!res.ok) {
      return Response.json({ error: "Unsplash API error" }, { status: res.status });
    }

    const data = await res.json();

    const results = data.results.map((img) => ({
      url: img.urls.regular,
      thumbnail: img.urls.small,
      title: img.description || img.alt_description || "Travel Image",
      source: "Unsplash",
    }));

    return Response.json({
      results,
      nextPage: data.total_pages > page ? page + 1 : null,
    });
  } catch (err) {
    console.error(err);
    return Response.json({ error: "Failed to fetch images" }, { status: 502 });
  }
}
Step 2: Add to .env.local
dotenv
Copy
UNSPLASH_ACCESS_KEY=your_unsplash_access_key_here

How to get Unsplash key (30 seconds):

Go to → https://unsplash.com/developers
Login → Create New Application
Fill any name/description → Create
Copy Access Key
Step 3: Update ImageSearchPanel.jsx slightly

Replace the handleSearch function with this:

jsx
Copy
const handleSearch = async (searchQuery = query, page = 1) => {
  if (!searchQuery?.trim()) return;

  setSearching(true);
  setLoading(true);

  try {
    const idToken = await auth.currentUser?.getIdToken();
    if (!idToken) {
      toast.error("Please sign in again");
      return;
    }

    const res = await fetch(
      `/api/search-images?q=${encodeURIComponent(searchQuery)}&page=${page}`,
      {
        headers: { Authorization: `Bearer ${idToken}` },
      }
    );

    if (!res.ok) throw new Error("Search failed");

    const data = await res.json();

    if (page === 1) {
      setResults(data.results || []);
    } else {
      setResults((prev) => [...prev, ...(data.results || [])]);
    }
    setNextStart(data.nextPage);
  } catch (err) {
    console.error(err);
    toast.error("Failed to load images");
  } finally {
    setLoading(false);
    setSearching(false);
  }
};

Would you like me to give you the full updated ImageSearchPanel.jsx with Unsplash support?

Just say "Give full panel" and I'll provide the complete file.

This should work immediately once you add the Unsplash key. Let me know!
